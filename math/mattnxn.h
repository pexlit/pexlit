#include "math/rectangle/rectangletn.h"
#include <math/quaternion/Quaternion.h>
// #include <immintrin.h> // Include SIMD header for Intel CPUs
#pragma once
/// <summary>
///
/// </summary>
/// <typeparam name="T"></typeparam>
/// <typeparam name="rows"></typeparam>
/// <typeparam name="cols">nested deeper</typeparam>
template <typename T, fsize_t rows, fsize_t cols>
// ordering in memory : 0 0, 0 1, 0 2, 1 0
//[0] [1] means from x to y
//[row] [col]
// x to x, x to y
// y to x, y to y
// 1 to x, 1 to y
//column major, so we can easily create a translation matrix for example
struct mattnxn : public vectn<vectn<T, cols>, rows>
{
	static constexpr fsize_t minimumSize = math::minimum(rows, cols);

	// standard: input = output
	constexpr mattnxn()
	{
		for (fsize_t i = 0; i < minimumSize; i++)
		{
			this->axis[i][i] = 1;
		}
	}
	template <typename t2, fsize_t cols2, fsize_t rows2>
	constexpr mattnxn(const std::array<vectn<t2, cols2>, rows2>& in) : vectn<vectn<T, cols>, rows>(in)
	{
	}

	template <typename t2, fsize_t cols2, fsize_t rows2>
	constexpr mattnxn(const vectn<vectn<t2, cols2>, rows2>& in) : vectn<vectn<T, cols>, rows>(in)
	{
	}

	inline static constexpr mattnxn empty()
	{
		return mattnxn(vectn<vectn<T, cols>, rows>());
	}

	constexpr mattnxn<T, cols, rows> transposed() const
	{
		mattnxn<T, cols, rows> result = mattnxn<T, cols, rows>();
		for (fsize_t i = 0; i < rows; i++)
		{
			for (fsize_t j = 0; j < cols; j++)
			{
				result[j][i] = this->axis[i][j];
			}
		}
		return result;
	}

	// glm method
	// https://codereview.stackexchange.com/questions/186770/4%C3%974-matrix-multiplication
	inline static constexpr mattnxn cross(const mattnxn& last, const mattnxn& first)
	{
		mattnxn mult = mattnxn::empty(); // all values are 0
		fsize_t col, firstRow, lastRow;
		for (firstRow = 0; firstRow < rows; ++firstRow)
			for (lastRow = 0; lastRow < rows; ++lastRow)
				for (col = 0; col < cols; ++col)
				{
					mult.axis[firstRow][col] += last.axis[lastRow][col] * first.axis[firstRow][lastRow];
				}
		return mult;
	}

	// from last to first: last = 0
	inline static mattnxn combine(const std::vector<mattnxn>& matrices)
	{
		auto i = matrices.begin();
		mattnxn out = *i;
		while (++i != matrices.end())
		{
			out = mattnxn::cross(out, *i);
		}
		return out;
	}

	constexpr mattnxn<T, math::maximum(rows, (fsize_t)2) - (fsize_t)1, math::maximum(cols, (fsize_t)2) - (fsize_t)1>
		coFactor(cvect2<fsize_t>& pos) const
		requires((rows > 1) && cols > 1)
	{
		mattnxn<T, math::maximum(rows, (fsize_t)2) - (fsize_t)1, math::maximum(cols, (fsize_t)2) - (fsize_t)1>
			result =
			mattnxn<T, math::maximum(rows, (fsize_t)2) - (fsize_t)1, math::maximum(cols, (fsize_t)2) - (fsize_t)1>();

		// Looping for each element of the matrix
		for (fsize_t row = 0, i = 0; row < rows; row++)
		{
			if (row != pos.x)
			{
				for (fsize_t col = 0, j = 0; col < cols; col++)
				{
					//  Copying into temporary matrix only those element
					//  which are not in given row and column
					if (col != pos.y)
					{
						result[i][j++] = this->axis[row][col];
					}
				}
				++i;
			}
		}
		return result;
	}

	// https://www.geeksforgeeks.org/adjoint-inverse-matrix/
	constexpr mattnxn
		adjoint() const
		requires((rows > 0) && (cols > 0))
	{
		mattnxn result = mattnxn();
		if constexpr (rows == 1)
		{
			result[0][0] = 1;
		}
		else
		{
			for (fsize_t i = 0; i < rows; i++)
			{
				for (fsize_t j = 0; j < cols; j++)
				{
					// Get cofactor of A[i][j]
					mattnxn<T,
						math::maximum(rows, (fsize_t)2) - (fsize_t)1,
						math::maximum(cols, (fsize_t)2) - (fsize_t)1>
						temp = coFactor(cvect2<fsize_t>(i, j));

					// sign of adj[j][i] positive if sum of row
					// and column indexes is even.
					cint& sign = ((i + j) % 2 == 0) ? 1 : -1;

					// Interchanging rows and columns to get the
					// transpose of the cofactor matrix
					result[j][i] = sign * temp.determinant();
				}
			}
		}
		return result;
	}

	// https://stackoverflow.com/questions/42802208/code-for-determinant-of-n-x-n-matrix#comment72802190_42804835
	constexpr T determinant() const
	{
		if constexpr (rows == 1)
		{
			return this->axis[0][0];
		}
		if constexpr (rows == 2)
		{
			return this->axis[0][0] * this->axis[1][1] - this->axis[0][1] * this->axis[1][0];
		}
		else
		{
			T result = 0;
			int sign = 1;

			for (fsize_t k = 0; k < rows; k++)
			{
				mattnxn<T, math::maximum(rows, (fsize_t)2) - (fsize_t)1, math::maximum(cols, (fsize_t)2) - (fsize_t)1>
					subMatrix = mattnxn<T, math::maximum(rows, (fsize_t)2) - (fsize_t)1, math::maximum(cols, (fsize_t)2) - (fsize_t)1>();

				for (fsize_t i = 1; i < rows; i++)
				{
					for (fsize_t aj = 0, bj = 0; aj < rows; aj++)
					{
						if (aj == k)
							continue;
						subMatrix[i - 1][bj] = this->axis[i][aj];
						++bj;
					}
				}

				result += sign * this->axis[0][k] * subMatrix.determinant();
				sign *= -1;
			}

			return result;
		}
	}

	// https://www.geeksforgeeks.org/adjoint-inverse-matrix/
	constexpr mattnxn inverse() const
	{
		// https://en.wikipedia.org/wiki/Inverse_matrix
		// https://www.youtube.com/watch?v=pKZyszzmyeQ
		const T& det = determinant();

		const mattnxn& adj = adjoint();

		return adj / det;
	}

	// if x++, output += getxstep
	template <fsize_t vectorSize = cols - 1>
	constexpr vectn<T, vectorSize> getStep(const axisID& axis) const
	{
		return vectn<T, vectorSize>(this->axis[(int)axis]);
	}

	// output at x 0 y 0
	constexpr vectn<T, math::maximum(cols, (fsize_t)2) - (fsize_t)1>
		getstart() const
		requires(cols > 1)
	{
		return this->axis[rows - 1];
	}

	template <fsize_t dimensionCount>
	constexpr static mattnxn translate(cvectn<T, dimensionCount>& add)
		requires(rows > 1)
	{
		mattnxn result = mattnxn();
		constexpr fsize_t minimumHeight = math::minimum(dimensionCount, cols - 1);

		for (fsize_t i = 0; i < minimumHeight; i++)
		{
			result[rows - 1][i] = add[i];
		}
		return result;
	}

	constexpr static mattnxn flip(cint& axis, cfp& offset)
	{
		mattnxn result = mattnxn();
		result[axis][axis] = -1;
		result[rows - 1][axis] = offset * 2;
		return result;
	}

	// clockwise
	// 0, 90, 180 or 270
	constexpr static mattnxn rotateDegrees(cint& angle)
	{
		cint sina = math::sinDegrees(angle);
		cint cosa = math::cosDegrees(angle);
		mattnxn result = mattnxn();
		result[0][0] = cosa;
		result[1][0] = sina;
		result[0][1] = -sina;
		result[1][1] = cosa;
		return result;
	}

	inline static mattnxn rotateDegrees(const vect2<T>& rotateAround, cint& angle)
	{
		return combine({
			// then move it back
			translate(rotateAround),
			// then rotate
			rotateDegrees(angle),
			// first set rotation point to 0,0
			translate(-rotateAround), });
	}
	// counterclockwise
	inline static mattnxn rotate(const T& angle)
	{
		const T& sina = sin(angle);
		const T& cosa = cos(angle);

		mattnxn result = mattnxn();
		result[0][0] = cosa;
		result[1][0] = -sina;
		result[0][1] = sina;
		result[1][1] = cosa;
		return result;
	}

	inline static mattnxn rotate(const vect2<T>& rotateAround, const T& angle)
	{
		return combine({// first set rotation point to 0,0
						translate(-rotateAround),

						// then rotate
						rotate(angle),
						// then move it back
						translate(rotateAround) });
	}


	inline static mattnxn rotate(Quaternion rotation)
		requires (rows == 4 && cols == 4)
	{
		rotation.normalize();
		//https://stackoverflow.com/questions/1556260/convert-quaternion-rotation-to-rotation-matrix
		return mattnxn<T, rows, cols>(vect4<vec4>
		{
			{
				(fp)1.0 - (fp)2.0 * rotation.y * rotation.y - (fp)2.0 * rotation.z * rotation.z, //x to x
					(fp)2.0 * rotation.x * rotation.y + (fp)2.0 * rotation.z * rotation.w, //x to y
					(fp)2.0 * rotation.x * rotation.z - (fp)2.0 * rotation.y * rotation.w, //x to z
					(fp)0.0//x to w
			},
			{
				(fp)2.0 * rotation.x * rotation.y - (fp)2.0 * rotation.z * rotation.w, //y to x
				(fp)1.0 - (fp)2.0 * rotation.x * rotation.x - (fp)2.0 * rotation.z * rotation.z, //y to y
				(fp)2.0 * rotation.y * rotation.z + (fp)2.0 * rotation.x * rotation.w, //y to z
				(fp)0.0 //y to w
			},
			{
					(fp)2.0 * rotation.x * rotation.z + (fp)2.0 * rotation.y * rotation.w, //z to x
				(fp)2.0 * rotation.y * rotation.z - (fp)2.0 * rotation.x * rotation.w, //z to y
				(fp)1.0 - (fp)2.0 * rotation.x * rotation.x - (fp)2.0 * rotation.y * rotation.y, //z to z
				(fp)0.0//z to w
			},
			{
				(fp)0.0,
				(fp)0.0,
				(fp)0.0,
				(fp)1.0
			}
		});
	}

	inline static mattnxn rotate3d(vect3<T> axis, const T& angle)
	{
		axis.normalize();
		T sinr = (T)sin(angle);
		T cosr = (T)cos(angle);
		T mincos = 1 - cosr;
		// https://en.wikipedia.org/wiki/Rotation_matrix#Rotation_matrix_from_axis_and_angle
		mattnxn result{};
		return mattnxn(vectn<vectn<T, cols>, rows>(
			vectn<T, cols>(
				cosr + axis.x * axis.x * mincos,
				axis.x * axis.y * mincos + axis.z * sinr,
				axis.x * axis.z * mincos - axis.y * sinr),
			vectn<T, cols>(
				axis.y * axis.x * mincos - axis.z * sinr,
				cosr + axis.y * axis.y * mincos,
				axis.y * axis.z * mincos + axis.x * sinr),
			vectn<T, cols>(
				axis.z * axis.x * mincos + axis.y * sinr,
				axis.z * axis.y * mincos - axis.x * sinr,
				cosr + axis.z * axis.z * mincos)
		));
		// axis.Normalize();
		// Matrix lhs = axis;
		// Matrix cross = Cross(lhs, matrix);
		// return cos * matrix + sin * cross;
	}

	template <fsize_t dimensionCount>
	inline static constexpr mattnxn scale(const vectn<T, dimensionCount> scalar)
	{
		constexpr fsize_t minimumSizeAndVector = math::minimum(dimensionCount, minimumSize);
		constexpr fsize_t maximumSizeAndVector = math::maximum(dimensionCount, minimumSize);
		mattnxn result = mattnxn();
		fsize_t i;
		for (i = 0; i < minimumSizeAndVector; i++)
		{
			result[i][i] = scalar[i];
		}
		for (; i < maximumSizeAndVector; i++)
		{
			result[i][i] = 1;
		}
		return result;
	}

	inline static constexpr mattnxn mirror(const axisID& axisIndex, const T& mirrorLocation)
	{
		vectn<T, rows - 1> translateAmount = vectn<T, rows - 1>();
		translateAmount[axisIndex] = mirrorLocation;

		vectn<T, rows - 1> scaleAmount = vectn<T, rows - 1>(1);
		scaleAmount[axisIndex] = -1;

		return cross(translate(translateAmount),		  // move back
			cross(scale(scaleAmount),			  // mirror
				translate(-translateAmount))); // move to mirror place
	}

	inline static constexpr mattnxn expandRectangle(const rectanglet2<T>& rect, const T& border)
	{
		return fromRectToRect(rect, rect.expanded(border));
	}
	template <fsize_t axisCount>
	inline static constexpr mattnxn fromRectToRect(const rectangletn<T, axisCount>& rectFrom, const rectangletn<T, axisCount>& rectTo)
	{
		const mattnxn translateFrom = translate(-rectFrom.pos0);   // move rectangle from pos0 of rectFrom to 0, 0
		const mattnxn scaled = scale(rectTo.size / rectFrom.size); // resize it
		const mattnxn translateTo = translate(rectTo.pos0);		   // move it to the pos0 of rectTo
		return cross(translateTo, cross(scaled, translateFrom));
	}
	inline static constexpr mattnxn fromPointsToPoints(const vect2<T>& a00, const vect2<T>& a10, const vect2<T>& a01, const vect2<T>& b00, const vect2<T>& b10, const vect2<T>& b01)
	{
		// from a->barycentric point->b
		return cross(baryCentric(b00, b10, b01).inverse(), baryCentric(a00, a10, a01));
	}

	inline static constexpr mattnxn fromRectToRotatedRect(const rectanglet2<T>& rectFrom, cint& angle, const rectanglet2<T>& rectTo)
	{
		mattnxn transform = mattnxn();
		if (angle == 90 || angle == 270)
		{
			transform = fromRectToRect(rectanglet2<T>(rectFrom.x + rectFrom.w * 0.5 - rectFrom.h * 0.5, rectFrom.y + rectFrom.h * 0.5 - rectFrom.w * 0.5, rectFrom.h, rectFrom.w), rectTo);
		}
		else
		{
			transform = fromRectToRect(rectanglet2<T>(rectFrom), rectTo);
		}
		if (angle != 0)
		{
			transform = cross(transform, rotateDegrees(vect2<T>(rectFrom.x + rectFrom.w * 0.5, rectFrom.y + rectFrom.h * 0.5), angle));
		}
		return transform;
	}

	constexpr bool isStraight() const
	{
		return (bool)this->axis[0].axis[0] ^ (bool)this->axis[1].axis[0] &&
			(bool)this->axis[0].axis[1] ^ (bool)this->axis[1].axis[1];
	}

	/*
	//https://franklinta.com/2014/09/08/computing-css-matrix3d-transforms/
	template<typename T>
	inline static mattnxn getTransform(const vect2<T> from[4], const vect2<T> to[4])
	{
		int yRowIndex = 0;

		Eigen::Matrix<T, 8, 8> A = Eigen::Matrix<T, 8, 8>();
		for (int i = 0; i < 4; i++)
		{
			const T rowXValues[8] = { from[i].x, from[i].y, 1, 0, 0, 0, -from[i].x * to[i].x, -from[i].y * to[i].x };
			A.row(yRowIndex++) = auto(rowXValues);
			const T rowYValues[8] = { 0, 0, 0, from[i].x, from[i].y, 1, -from[i].x * to[i].y, -from[i].y * to[i].y };
			A.setYRow(yRowIndex++, rowYValues);
		}
		int yRowIndex = 0;
		glm<T, 8, 1> b = mattnxn<T, 8, 1>();
			for (int i = 0; i < 4; i++)
			{
				b.values[yRowIndex++] = to[i].x;
				b.values[yRowIndex++] = to[i].x;

					# Solve A* h = b for h
					h = numeric.solve(A, b)

					H = [[h[0], h[1], 0, h[2]],
					[h[3], h[4], 0, h[5]],
					[0, 0, 1, 0],
					[h[6], h[7], 0, 1]]

					# Sanity check that H actually maps `from` to `to`
					for i in[0 ... 4]
						lhs = numeric.dot(H, [from[i].x, from[i].y, 0, 1])
						k_i = lhs[3]
						rhs = numeric.dot(k_i, [to[i].x, to[i].y, 0, 1])
						console.assert(numeric.norm2(numeric.sub(lhs, rhs)) < 1e-9, "Not equal:", lhs, rhs)
						H

						applyTransform = (element, originalPos, targetPos, callback) ->
						# All offsets were calculated relative to the document
						# Make them relative to(0, 0) of the element instead
						from = for p in originalPos
						x : p[0] - originalPos[0][0]
						y : p[1] - originalPos[0][1]
						to = for p in targetPos
						x : p[0] - originalPos[0][0]
						y : p[1] - originalPos[0][1]

						# Solve for the transform
						H = getTransform(from, to)

						# Apply the matrix3d as H transposed because matrix3d is column major order
						# Also need use toFixed because css doesn'T allow scientific notation
						$(element).css
						'transform': "matrix3d(#{(H[j][i].toFixed(20) for j in [0...4] for i in [0...4]).join(',')})"
						'transform-origin' : '0 0'

						callback ? (element, H)

						makeTransformable = (selector, callback) ->
						$(selector).each(i, element) ->
						$(element).css('transform', '')

						# Add four dots to corners of `element` as control points
						controlPoints = for position in['left top', 'left bottom', 'right top', 'right bottom']
						$('<div>')
						.css
						border : '10px solid black'
						borderRadius : '10px'
						cursor : 'move'
						position : 'absolute'
						zIndex : 100000
						.appendTo 'body'
						.position
						at : position
						of : element
						collision : 'none'

						# Record the original positions of the dots
						originalPos = ([p.offset().left, p.offset().top] for p in controlPoints)

						# Transform `element` to match the new positions of the dots whenever dragged
						$(controlPoints).draggable
						start : = >
						$(element).css('pointer-events', 'none') # makes dragging around iframes easier
						drag : = >
						applyTransform(element, originalPos, ([p.offset().left, p.offset().top] for p in controlPoints), callback)
						stop: = >
						applyTransform(element, originalPos, ([p.offset().left, p.offset().top] for p in controlPoints), callback)
						$(element).css('pointer-events', 'auto')

						element

						makeTransformable('.box', (element, H) ->
							console.log($(element).css('transform'))
							$(element).html(
								$('<table>')
								.append(
									$('<tr>').html(
										$('<td>').text('matrix3d(')
									)
								)
								.append(
									for i in[0 ... 4]
										$('<tr>').append(
											for j in[0 ... 4]
												$('<td>').text(H[j][i] + if i == j == 3 then '' else ',')
												)
										)
								.append(
									$('<tr>').html(
										$('<td>').text(')')
									)
								)
												)
						)


		throw "implement with eigen";

	}
	*/

	// Compute barycentric set with respect to triangle (p00, p10, p01)
	// multiply a vect2<T> by the returned matrix and get a vect2<T>(v, w)
	inline static constexpr mattnxn baryCentric(const vect2<T>& p00, const vect2<T>& p10, const vect2<T>& p01)
	{
		cvect2<T>& v0 = p10 - p00, v1 = p01 - p00,
			v2a00 = -p00, v2a10 = vect2<T>(1, 0) - p00, v2a01 = vect2<T>(0, 1) - p00; // calculate for (0,0),(1,0),(0,1)
		const T& d00 = vect2<T>::dot(v0, v0);
		const T& d01 = vect2<T>::dot(v0, v1);
		const T& d11 = vect2<T>::dot(v1, v1);
		const T& d20a00 = vect2<T>::dot(v2a00, v0);
		const T& d21a00 = vect2<T>::dot(v2a00, v1);
		const T& d20a10 = vect2<T>::dot(v2a10, v0);
		const T& d21a10 = vect2<T>::dot(v2a10, v1);
		const T& d20a01 = vect2<T>::dot(v2a01, v0);
		const T& d21a01 = vect2<T>::dot(v2a01, v1);
		const T& denom = d00 * d11 - d01 * d01;
		const T& div = 1.0 / denom;
		const T& va00 = (d11 * d20a00 - d01 * d21a00) * div; // v at x 0 y 0
		const T& wa00 = (d00 * d21a00 - d01 * d20a00) * div; // w at x 0 y 0
		const T& va10 = (d11 * d20a10 - d01 * d21a10) * div; // v at x 1 y 0
		const T& wa10 = (d00 * d21a10 - d01 * d20a10) * div; // w at x 1 y 0
		const T& va01 = (d11 * d20a01 - d01 * d21a01) * div; // v at x 0 y 1
		const T& wa01 = (d00 * d21a01 - d01 * d20a01) * div; // w at x 0 y 1
		// multx, multy, plus
		return mattnxn(
			va10 - va00, va01 - va00, va00, // v
			wa10 - wa00, wa01 - wa00, wa00, // w
			0, 0, 1							// empty
		);
	}

	// will bring z to the range [0,1] with 0 being zFar and 1 being zNear (so higher than 1 should be clipped)
	// will bring x and y in the range [-1, 1]
	// glm method
	//fovY: fov of the screens y axis in radians
	inline static constexpr mattnxn perspectiveFov(const T& fovY, const vectn<T, 2>& screenSize, const T& zNear, const T& zFar)
	{
		const T& h = cos((fp)0.5 * fovY) / sin((fp)0.5 * fovY);
		const T& w = h * screenSize.y / screenSize.x; /// todo max(width , Height) / min(width , Height)?

		mattnxn result = empty();
		result[0][0] = w;								 // the width multiplier of the screen
		result[1][1] = h;								 // the height multiplier of the screen
		result[2][2] = -(zFar + zNear) / (zFar - zNear); // the z multiplier
		// to bring it in range(0,1):
		result[2][3] = -static_cast<fp>(1);									  // the distance multiplier
		result[3][2] = -(static_cast<fp>(2) * zFar * zNear) / (zFar - zNear); // the distance substractor
		// row followed by column
		return result;
	}

	// up: normal
	// eye: position of the camera
	// center: looking at
	// the view matrix
	// glm method
	// right handed!
	// https://stackoverflow.com/questions/19740463/lookat-function-im-going-crazy
	// https://stackoverflow.com/questions/21830340/understanding-glmlookat
	inline static constexpr mattnxn lookat(cvect3<T>& eye, cvect3<T>& center, cvect3<T>& up)
	{
		// Create a new coordinate system
		cvect3<T>& screenz((center - eye).normalized());			   // the look direction
		cvect3<T>& screenx(vect3<T>::cross(screenz, up).normalized()); // the sideways direction
		//we need to calculate again, because up isn't guaranteed to be perpendicular to screenz, because the center can be anywhere.
		cvect3<T>& screeny(vect3<T>::cross(screenx, screenz));		   // the upwards direction.

		mattnxn result = mattnxn();
		result[0][0] = screenx.x; // the new x
		result[1][0] = screenx.y;
		result[2][0] = screenx.z;
		result[0][1] = screeny.x; // the new y
		result[1][1] = screeny.y;
		result[2][1] = screeny.z;
		result[0][2] = -screenz.x; // the new z
		result[1][2] = -screenz.y;
		result[2][2] = -screenz.z;
		result[3][0] = -vect3<T>::dot(screenx, eye);
		result[3][1] = -vect3<T>::dot(screeny, eye);
		result[3][2] = vect3<T>::dot(screenz, eye);
		return result;
	}

	constexpr rectanglet2<T> getTransformedRectangleBounds(const rectanglet2<T>& rect) const
	{
		const vect2<T> pos00 = multPointMatrix(rect.pos0); // upper left corner
		const vect2<T> pos10 = multPointMatrix(rect.pos10());
		const vect2<T> pos01 = multPointMatrix(rect.pos01());
		const vect2<T> pos11 = multPointMatrix(rect.pos1());

		rectanglet2<T> boundaries;
		boundaries.pos0 = vect2<T>(
			math::minimum(math::minimum(pos00.x, pos10.x), math::minimum(pos01.x, pos11.x)),
			math::minimum(math::minimum(pos00.y, pos10.y), math::minimum(pos01.y, pos11.y)));
		boundaries.size = vect2<T>(
			math::maximum(math::maximum(pos00.x, pos10.x), math::maximum(pos01.x, pos11.x)),
			math::maximum(math::maximum(pos00.y, pos10.y), math::maximum(pos01.y, pos11.y))) -
			boundaries.pos0;
		return boundaries;
	}

	// template <fsize_t inputAxisCount = cols, fsize_t outputAxisCount = inputAxisCount>
	// constexpr vectn<T, outputAxisCount> multPointMatrix(const vectn<T, inputAxisCount> &in) const;

	// template<fsize_t inputAxisCount, fsize_t outputAxisCount>
	inline constexpr vectn<T, 2> multPointMatrix(const vectn<T, 2>& in) const
		requires(rows == 3 && cols == 3)
	{
		return vectn<T, 2>(
			in.axis[0] * this->axis[0].axis[0] + in.axis[1] * this->axis[1].axis[0] + this->axis[2].axis[0],
			in.axis[0] * this->axis[0].axis[1] + in.axis[1] * this->axis[1].axis[1] + this->axis[2].axis[1]);
	}
	template <fsize_t outputAxisCount = cols, fsize_t inputAxisCount>
	inline constexpr vectn<T, outputAxisCount> multPointMatrix(const vectn<T, inputAxisCount>& in) const
	{
		vectn<T, outputAxisCount> result = in.axis[0] * this->axis[0];
		fsize_t fromIndex = 1;
		constexpr fsize_t colCount = math::minimum(outputAxisCount, cols);
		constexpr fsize_t rowCount = math::minimum(inputAxisCount, rows);
		for (; fromIndex < rowCount; fromIndex++)
		{
			for (fsize_t colIndex = 0; colIndex < colCount; colIndex++) {

				result[colIndex] += in.axis[fromIndex] * this->axis[fromIndex][colIndex];
			}
		}
		// the extra 1's to add to the vector
		for (; fromIndex < cols; fromIndex++)
		{
			for (fsize_t colIndex = 0; colIndex < colCount; colIndex++) {

				result[colIndex] += this->axis[fromIndex][colIndex];
			}
		}
		return result;
	}

	// multpointmatrix, but without the translate
	template <fsize_t axisCount>
	constexpr vectn<T, axisCount> multSizeMatrix(const vectn<T, axisCount>& size) const
	{
		vectn<T, axisCount> result = vectn<T, axisCount>();
		for (fsize_t toIndex = 0; toIndex < axisCount; toIndex++)
		{
			for (fsize_t fromIndex = 0; fromIndex < axisCount; fromIndex++)
			{
				result[toIndex] += size[fromIndex] * this->axis[fromIndex][toIndex];
			}
		}
		return result;
	}

	constexpr rectangle2 multRectMatrix(crectangle2& in) const
	{
		return rectangle2(multPointMatrix(in.pos0), multSizeMatrix(in.size));
	}
	constexpr rectangle2 multRotatedRectMatrix(crectangle2& in) const
	{
		return rectangle2::fromOppositeCorners(multPointMatrix(in.pos0), multPointMatrix(in.pos1()));
	}
};

typedef mattnxn<int, 3, 3> mat3x3i;

typedef mattnxn<fp, 1, 1> mat1x1;
typedef mattnxn<fp, 2, 2> mat2x2;
typedef mattnxn<fp, 3, 3> mat3x3;
typedef mattnxn<fp, 4, 4> mat4x4;

typedef const mat3x3 cmat3x3;
typedef const mat3x3i cmat3x3i;

// constexpr mat3x3 testMatrix = mat3x3(
//	vectn<vec3, 3>(
//		vec3(2, -3, 1),
//		vec3(2, 0, -1),
//		vec3(1, 4, 5)
//		));
//
// constexpr vec2 multiplied = testMatrix.multPointMatrix(cvec2(0, 0));
//
// constexpr fp testDet = testMatrix.determinant();
// constexpr mat3x3 testAdj = testMatrix.adjoint();
// constexpr mat3x3 inverseTestMatrix = testMatrix.inverse();
// constexpr mat3x3 testInv = testAdj / testDet;
// static const mat2x2 testCoFactor = testMatrix.coFactor(vectn<fsize_t, 2>(0, 2));
