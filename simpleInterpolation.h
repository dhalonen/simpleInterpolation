/*
 * Copyright (c) 2017 David C. Halonen  
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
//This pragma allows for divide by zero & overflow testing
#pragma STDC FENV_ACCESS ON

namespace simpleTools
{
    template <class X, class Y>
    class interpolation {
    public:
        explicit interpolation( std::shared_ptr< std::vector< std::pair< X, Y > > >a ): intrpData( a ) {};

        //The simplest interpolation is to return the closest Y to a given X.
        Y nearestY( X x )
        {
            if( preflightFailed() ) return nan( "NAN" );

            //if leftX > x, then leftX is the closest
            if( leftX > x )
            {
                return leftY;
            }

        }

        //given interpolation point, x, compute it's corresponding y value
        Y getY( X x )
        {
            if( preflightFailed() ) return nan( "NAN" );

            //if leftX > x, then the interpolation point is to the left of the table.
            //compute y = mx + b, using the 1st two pairs to determine that equation
            if( leftX > x ){
                ++head;
                rightX = head->first;
                rightY = head->second;
                return interpolateOnSegment( x );
            }

            auto next = ++head;
            rightX = next->first;
            rightY = next->second;

            //scan pairs to determine where the desired point lies between
            for( std::pair< X, Y >&item : *intrpData ){
                if( x == leftX ) return leftY;
                if( x == rightX ) return rightY;

                if( item.first > x ){    //we have the rhs
                    rightX = item.first;
                    rightY = item.second;
                    break;
                } else {
                    leftX = item.first;
                    leftY = item.second;
                }
            }

            //if rightX <  x, then the interpolation point is to the right of the table.
            //compute y = mx + b, using the last two pairs to determine that equation
            if( x > rightX ) {
                //perform a reverse iteration.
                auto rhead = intrpData->rbegin();
                rightX = rhead->first;
                rightY = rhead->second;
                ++rhead;
                leftX = rhead->first;
                leftY = rhead->second;
                return interpolateOnSegment( x );
            }

            //simply perform linear interpolation between two points
            return interpolate( x );
        }

    private:
        std::shared_ptr< std::vector< std::pair< X, Y > > > intrpData;
        X leftX, leftY; //current left data point
        Y rightX, rightY; //next adjacent data point
        typename std::vector< std::pair< X, Y >>::iterator head;

        bool preflightFailed()
        {
            //If less then 2 pairs, then nothing can be done.
            if( intrpData->size() < 2 ) return true;

            head = intrpData->begin();
            auto end = intrpData->end();
            if( head == end ) return true;  //empty vector

            head = intrpData->begin();
            leftX = head->first;                    //start from left side of graph or top of table
            leftY = head->second;

            return false;
        }

        Y interpolate( X x)
        {
            Y slope = computeSlope();
            if( std::isnan( slope )) return slope;

            std::feclearexcept(FE_ALL_EXCEPT);
            Y result = leftY + (x - leftX) * slope;
            if(std::fetestexcept(FE_OVERFLOW)) return nan( "NAN" ); 

            return result; 
        };

        Y interpolateOnSegment( X x)
        {   // y = mx + b
            Y m = computeSlope();
            if( std::isnan( m )) return m;
            Y b = leftY - m * leftX;

            std::feclearexcept(FE_ALL_EXCEPT);
            Y result = m * x + b;
            if(std::fetestexcept(FE_OVERFLOW)) return nan( "NAN" ); 

            return result; 
        };

        Y computeSlope()
        {
            Y denominator = rightX - leftX;

            std::feclearexcept(FE_ALL_EXCEPT);
            Y result = ( rightY - leftY ) / denominator;
            if(std::fetestexcept(FE_DIVBYZERO)) return nan( "NAN" ); 

            return result; 
        }
    };
}
