/*
 * Copyright (c) 2017-2021 David C. Halonen  
 * The MIT License
 * Permission is hereby granted, free of charge, to any person obtaining a copy of 
 * this software and associated documentation files (the "Software"), to deal in 
 * the Software without restriction, including without limitation the rights to use, 
 * copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the 
 * Software, and to permit persons to whom the Software is furnished to do so, subject 
 * to the following conditions: 
 *
 *   The above copyright notice and this permission notice shall be included 
 *   in all copies or substantial portions of the Software.  
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
 * PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <vector>
#include <tuple>
#include <cfenv>
#include <memory>
#include <cmath>
#include <tuple>

namespace simpleTools {
    enum class InterpolationResultType {
        OK,
        lessThanData,
        greaterThanData,
        exactMatch,
        dataUnsorted,
        dataIncomplete,
        divideByZero
    };

    template<class X, class Y>
    class interpolation {
    public:
        explicit interpolation(const std::shared_ptr<std::vector<std::pair<X, Y> > > a) : intrpData(a) {}

        //The simplest interpolation is to return the closest Y to a given X.
        std::tuple<InterpolationResultType, Y> nearestY(X x) {
            InterpolationResultType preflightResult = preflightFailed();
            if (preflightResult != InterpolationResultType::OK) return {preflightResult, 0};

            //if leftX > x, then leftX is the closest
            if (leftX > x) {
                return {InterpolationResultType::lessThanData, leftY};
            }

            std::tuple<InterpolationResultType, Y> scanResult = scanVector(x);
            if (std::get<0>(scanResult) == InterpolationResultType::exactMatch) {
                return {InterpolationResultType::OK, std::get<1>(scanResult)};
            }

            if (std::get<0>(scanResult) == InterpolationResultType::dataUnsorted) {
                return {InterpolationResultType::dataUnsorted, 0};
            }

            //if rightX <  x, then the interpolation point is to the right of the table.
            if (x > rightX) {
                return {InterpolationResultType::greaterThanData, intrpData->rbegin()->second};
            }

            //find the closest X to x and return that Y
            if (abs(x - leftX) < abs(x - rightX)) {
                return {InterpolationResultType::OK, leftY};
            }
            return {InterpolationResultType::OK, rightY};
        }

        //given interpolation point, x, compute it's corresponding y value
        std::tuple<InterpolationResultType, Y> getY(X x) {
            InterpolationResultType preflightResult = preflightFailed();
            if (preflightResult != InterpolationResultType::OK) return {preflightResult, 0};

            //if leftX > x, then the interpolation point is to the left of the table.
            //compute y = mx + b, using the 1st two pairs to determine that equation
            if (leftX > x) {
                ++head;
                rightX = head->first;
                rightY = head->second;
                return interpolateOnSegment(x);
            }

            std::tuple<InterpolationResultType, Y> scanResult = scanVector(x);
            if (std::get<0>(scanResult) == InterpolationResultType::exactMatch) {
                return {InterpolationResultType::OK, std::get<1>(scanResult)};
            }

            if (std::get<0>(scanResult) == InterpolationResultType::dataUnsorted) {
                return {InterpolationResultType::dataUnsorted, 0};
            }

            //if rightX <  x, then the interpolation point is to the right of the table.
            //compute y = mx + b, using the last two pairs to determine that equation
            if (x > rightX) {
                //perform a reverse iteration.
                auto rhead = intrpData->rbegin();
                rightX = rhead->first;
                rightY = rhead->second;
                ++rhead;
                leftX = rhead->first;
                leftY = rhead->second;
                return interpolateOnSegment(x);
            }

            //simply perform linear interpolation between two points
            return interpolate(x);
        }

    private:
        std::shared_ptr<std::vector<std::pair<X, Y> > > intrpData;
        X rightX, leftX; //current left data point
        Y rightY, leftY; //next adjacent data point
        typename std::vector<std::pair<X, Y > >::iterator head;

        InterpolationResultType preflightFailed() {
            head = intrpData->begin();
            if (head == intrpData->end()) return InterpolationResultType::dataIncomplete;  //empty vector

            //If less then 2 pairs, then nothing can be done.
            if (intrpData->size() < 2) return InterpolationResultType::dataIncomplete;

            head = intrpData->begin();
            leftX = head->first;                    //start from left side of graph or top of table
            leftY = head->second;

            return InterpolationResultType::OK;
        }

        std::tuple<InterpolationResultType, Y> scanVector(X x) {
            auto currX = head->first;   //current X under consideration
            auto next = ++head;
            rightX = next->first;
            rightY = next->second;

            //scan pairs to determine where the desired point lies between
            auto foundRhs = false;  //need to "peek" ahead by one to ensure sorted data
            for (std::pair<X, Y> &item : *intrpData) {
                if (item.first < currX) {
                    return std::make_tuple(InterpolationResultType::dataUnsorted, 0);
                } else {
                    currX = item.first;
                }
                if (x == leftX) return std::make_tuple(InterpolationResultType::exactMatch, leftY);
                if (x == rightX) return std::make_tuple(InterpolationResultType::exactMatch, rightY);

                if (item.first > x) {    //we have the rhs
                    //This flag looks ahead by one item. If the 'dataUnsorted' hasn't been tripped, exit the loop.
                    if (foundRhs) {
                        break;
                    } else {
                        rightX = item.first;
                        rightY = item.second;
                        foundRhs = true;
                    }
                } else {
                    leftX = item.first;
                    leftY = item.second;
                }
            }
            return std::make_tuple(InterpolationResultType::OK, 0); //just return OK, caller will examine state
        }

        std::tuple<InterpolationResultType, Y> interpolate(X x) {
            std::tuple<simpleTools::InterpolationResultType, Y> result = computeSlope();
            if (std::get<0>(result) == InterpolationResultType::divideByZero) {
                return result;
            }

            Y slope = std::get<1>(result);

            return std::make_tuple(InterpolationResultType::OK, leftY + (x - leftX) * slope);
        }

        std::tuple<InterpolationResultType, Y> interpolateOnSegment(X x) {   // y = mx + b
            std::tuple<simpleTools::InterpolationResultType, Y> result = computeSlope();
            if (std::get<0>(result) == InterpolationResultType::divideByZero) {
                return result;
            }
            Y m = std::get<1>(result);
            Y b = leftY - m * leftX;

            return std::make_tuple(InterpolationResultType::OK, m * x + b);
        }

        std::tuple<InterpolationResultType, Y> computeSlope() {
            Y denominator = rightX - leftX;

            if (abs(denominator) < (Y) 0.0001) {
                return {InterpolationResultType::divideByZero, 0};
            }

            return std::make_tuple(InterpolationResultType::OK, (rightY - leftY) / denominator);
        }
    };
}
