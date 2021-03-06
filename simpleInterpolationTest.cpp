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
#define CATCH_CONFIG_MAIN

#include <vector>

/* Unit Test Framework: https://github.com/philsquared/Catch, release v1.8.2 */
#include "catch.hpp"

#include "simpleInterpolation.h"

TEST_CASE("First test") {
    std::shared_ptr<std::vector<std::pair<double, double> > > graphData
    (new std::vector<std::pair<double, double> >
        (
        {
                {1.0,  1.0},
                {2.5,  1.3},
                {3.0,  2.0},
                {3.75, 0.5},
                {4.1,  2.25},
                {5.0,  1.75},
                {5.3,  1.9}
            }
        )
    );
    simpleTools::interpolation<double, double> graphDataIntrp(graphData, 0.001);
    std::tuple<simpleTools::InterpolationResultType, double> result;

    result = graphDataIntrp.getY(1.75);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(1.15).epsilon(0.01));

    result = graphDataIntrp.getY(2.75);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(1.65).epsilon(0.01));

    result = graphDataIntrp.getY(3.375);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(1.25).epsilon(0.01));

    result = graphDataIntrp.getY(3.925);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(1.375).epsilon(0.01));

    result = graphDataIntrp.getY(4.55);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(2.0).epsilon(0.01));

    result = graphDataIntrp.getY(5.15);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(1.825).epsilon(0.01));

    result = graphDataIntrp.getY(0.0);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(0.8).epsilon(0.01));

    result = graphDataIntrp.getY(6);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(2.25).epsilon(0.01));

    result = graphDataIntrp.getY(3.75);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(0.5).epsilon(0.01));

    result = graphDataIntrp.getY(3.0);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(2.0).epsilon(0.01));

    std::shared_ptr<std::vector<std::pair<double, double> > > emptyData(
            new std::vector<std::pair<double, double> >({}));   //empty vector check
    simpleTools::interpolation<double, double> emptyIntrp(emptyData, 0.1);
    result = emptyIntrp.getY(100);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::dataIncomplete);
    REQUIRE(std::get<1>(result) == Approx(0.0).epsilon(0.01));

    emptyData->push_back({1.0, 1.0});    //cannot work with 1 pair of data
    simpleTools::interpolation<double, double> singlePair(emptyData, 0.1);
    result = emptyIntrp.getY(100);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::dataIncomplete);
    REQUIRE(std::get<1>(result) == Approx(0.0).epsilon(0.01));

    std::shared_ptr<std::vector<std::pair<double, double> > > zeroData(new std::vector<std::pair<double, double> >(
            {{0, 0},
             {0, 0}}));   //return nan when divide by zero
    simpleTools::interpolation<double, double> zeroIntrp(zeroData, 0.1);
    result = zeroIntrp.getY(100);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::divideByZero);
    REQUIRE(std::get<1>(result) == Approx(0.0).epsilon(0.01));
}

TEST_CASE("Long double test") {
    std::tuple<simpleTools::InterpolationResultType, long double> result;

    std::shared_ptr<std::vector<std::pair<long double, long double> > > longDouble
    (
    new std::vector<std::pair<long double, long double> >(
    {
            {1.0, 9.1},
            {2.0, 8.2},
            {3.0, 7.3},
            {4.0, 6.4},
            {5.0, 5.5},
            {6.0, 4.6},
            {7.0, 3.7},
            {8.0, 2.8},
            {9.0, 1.9}
        } )
    );
    simpleTools::interpolation<long double, long double> ldIntrp(longDouble, 0.01);
    result = ldIntrp.getY(static_cast<long double> (1.5));
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(8.65));
}

TEST_CASE("Float test") {
    std::tuple<simpleTools::InterpolationResultType, float> result;

    std::shared_ptr<std::vector<std::pair<float, float> > > floatData
    (
    new std::vector<std::pair<float, float> >(
     {
            {1.0, 9.1},
            {2.0, 8.2},
            {3.0, 7.3},
            {4.0, 6.4},
            {5.0, 5.5},
            {6.0, 4.6},
            {7.0, 3.7},
            {8.0, 2.8},
            {9.0, 1.9}
        } )
    );
    simpleTools::interpolation<float, float> floatIntrp(floatData, static_cast<float> (0.01));
    result = floatIntrp.getY(1.5);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(8.65).epsilon(0.01));
}

TEST_CASE("Dis-similar test") {
    std::shared_ptr<std::vector<std::pair<int, double> > > disData
    (
    new std::vector<std::pair<int, double> > (
    {
            {1, 1.0},
            {3, 2.0},
            {5, 1.75}
        } )
    );
    simpleTools::interpolation<int, double> disDataIntrp(disData, 0);
    std::tuple<simpleTools::InterpolationResultType, double> result;

    result = disDataIntrp.getY(2);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(1.5).epsilon(0.01));

    result = disDataIntrp.getY(4);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(1.875).epsilon(0.01));

    result = disDataIntrp.getY(6);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(1.625).epsilon(0.01));
}

TEST_CASE("Nearest test") {
    std::shared_ptr<std::vector<std::pair<double, double> > > nearData
    (
    new std::vector<std::pair<double, double> > (
    {
            {1.0,  1.0},
            {2.5,  1.3},
            {3.0,  2.0},
            {3.75, 0.5},
            {4.1,  2.25},
            {5.0,  1.75},
            {5.3,  1.9}
       } )
    );
    simpleTools::interpolation<double, double> nearDataIntrp(nearData, 0.01);
    std::tuple<simpleTools::InterpolationResultType, double> result;
    result = nearDataIntrp.nearestY(0.75);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::lessThanData);
    REQUIRE(std::get<1>(result) == Approx(1.0).epsilon(0.01));

    result = nearDataIntrp.nearestY(6);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::greaterThanData);
    REQUIRE(std::get<1>(result) == Approx(1.9).epsilon(0.01));

    result = nearDataIntrp.nearestY(2.749);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(1.3).epsilon(0.01));

    result = nearDataIntrp.nearestY(2.750);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(2.0).epsilon(0.01));

    result = nearDataIntrp.nearestY(2.751);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::OK);
    REQUIRE(std::get<1>(result) == Approx(2.0).epsilon(0.01));

    std::shared_ptr<std::vector<std::pair<double, double> > > emptyData(new std::vector<std::pair<double, double> >
                                                                                ({}));   //empty vector check
    simpleTools::interpolation<double, double> emptyIntrp(emptyData, 0.1);
    result = emptyIntrp.nearestY(100);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::dataIncomplete);
    REQUIRE(std::get<1>(result) == Approx(0.0).epsilon(0.01));

    emptyData->push_back({1.0, 1.0});    //cannot work with 1 pair of data
    simpleTools::interpolation<double, double> singlePair(emptyData, 0.1);
    result = emptyIntrp.nearestY(100);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::dataIncomplete);
    REQUIRE(std::get<1>(result) == Approx(0.0).epsilon(0.01));
}


TEST_CASE("Unsorted data test") {
    std::shared_ptr<std::vector<std::pair<double, double> > > nearData
    (
    new std::vector<std::pair<double, double> > (
    {
            {1.0,  1.0},
            {2.5,  1.3},
            {5.0,  2.0}, /* This is a bad row */
            {3.75, 0.5},
            {4.1,  2.25},
            {5.3,  1.9}
        } )
    );
    simpleTools::interpolation<double, double> nearDataIntrp(nearData, 0.1);
    std::tuple<simpleTools::InterpolationResultType, double> result;
    result = nearDataIntrp.nearestY(4);
    REQUIRE(std::get<0>(result) == simpleTools::InterpolationResultType::dataUnsorted);
}
