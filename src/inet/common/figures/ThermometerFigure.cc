//
// Copyright (C) 2016 OpenSim Ltd
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//
//

#include "ThermometerFigure.h"
#include <cstdlib>

//TODO namespace inet { -- for the moment commented out, as OMNeT++ 5.0 cannot instantiate a figure from a namespace
using namespace inet;

Register_Class(ThermometerFigure);

#if OMNETPP_VERSION >= 0x500

static const double CONTAINER_LINE_WIDTH_PERCENT = 0.01;  //TODO Misi: if I change this to 0.05, the whole figure falls apart!
static const double TICK_LINE_WIDTH_PERCENT = 0.005;  //TODO apparently unused?
static const double TICK_LENGTH_PERCENT = 0.2;
static const double CONTAINER_POS_PERCENT = 0.05;
static const double CONTAINER_WIDTH_PERCENT = 0.35;
static const double CONTAINER_OFFSET_PERCENT = 0.02;

static const char *PKEY_MERCURY_COLOR = "mercuryColor";
static const char *PKEY_LABEL = "label";
static const char *PKEY_LABEL_FONT = "labelFont";
static const char *PKEY_LABEL_COLOR = "labelColor";
static const char *PKEY_MIN_VALUE = "min";
static const char *PKEY_MAX_VALUE = "max";
static const char *PKEY_TICK_SIZE = "tickSize";
static const char *PKEY_POS = "pos";
static const char *PKEY_SIZE = "size";
static const char *PKEY_ANCHOR = "anchor";
static const char *PKEY_BOUNDS = "bounds";

ThermometerFigure::ThermometerFigure(const char *name) : cGroupFigure(name)
{
    addChildren();
}

ThermometerFigure::~ThermometerFigure()
{
    // delete figures which is not in canvas
    for(int i = numTicks; i < tickFigures.size(); ++i)
    {
        delete tickFigures[i];
        delete numberFigures[i];
    }
}

cFigure::Rectangle ThermometerFigure::getBounds() const
{
    return bounds;
}

void ThermometerFigure::setBounds(Rectangle rect)
{
    bounds = rect;
    layout();
}

cFigure::Color ThermometerFigure::getMercuryColor() const
{
    return mercuryFigure->getLineColor();
}

void ThermometerFigure::setMercuryColor(cFigure::Color color)
{
    mercuryFigure->setFillColor(color);
}

const char* ThermometerFigure::getLabel() const
{
    return labelFigure->getText();
}

void ThermometerFigure::setLabel(const char *text)
{
    labelFigure->setText(text);
}

cFigure::Font ThermometerFigure::getLabelFont() const
{
    return labelFigure->getFont();
}

void ThermometerFigure::setLabelFont(cFigure::Font font)
{
    labelFigure->setFont(font);
}

cFigure::Color ThermometerFigure::getLabelColor() const
{
    return labelFigure->getColor();
}

void ThermometerFigure::setLabelColor(cFigure::Color color)
{
    labelFigure->setColor(color);
}

double ThermometerFigure::getMin() const
{
    return min;
}

void ThermometerFigure::setMin(double value)
{
    if(min != value)
    {
        min = value;
        redrawTicks();
        refresh();
    }
}

double ThermometerFigure::getMax() const
{
    return max;
}

void ThermometerFigure::setMax(double value)
{
    if(max != value)
    {
        max = value;
        redrawTicks();
        refresh();
    }
}

double ThermometerFigure::getTickSize() const
{
    return tickSize;
}

void ThermometerFigure::setTickSize(double value)
{
    if(tickSize != value)
    {
        tickSize = value;
        redrawTicks();
        refresh();
    }
}

void ThermometerFigure::parse(cProperty *property)
{
    cGroupFigure::parse(property);
    setBounds(parseBounds(property));

    // Set default
    redrawTicks();

    const char *s;
    if ((s = property->getValue(PKEY_MERCURY_COLOR)) != nullptr)
        setMercuryColor(parseColor(s));
    if ((s = property->getValue(PKEY_LABEL)) != nullptr)
        setLabel(s);
    if ((s = property->getValue(PKEY_LABEL_FONT)) != nullptr)
        setLabelFont(parseFont(s));
    if ((s = property->getValue(PKEY_LABEL_COLOR)) != nullptr)
        setLabelColor(parseColor(s));
    // This must be initialize before min and max because it is possible to be too much unnecessary tick and number
    if ((s = property->getValue(PKEY_TICK_SIZE)) != nullptr)
        setTickSize(atof(s));
    if ((s = property->getValue(PKEY_MIN_VALUE)) != nullptr)
        setMin(atof(s));
    if ((s = property->getValue(PKEY_MAX_VALUE)) != nullptr)
        setMax(atof(s));
}

const char **ThermometerFigure::getAllowedPropertyKeys() const
{
    static const char *keys[32];
    if (!keys[0]) {
        const char *localKeys[] = {PKEY_MERCURY_COLOR, PKEY_LABEL, PKEY_LABEL_FONT,
                                   PKEY_LABEL_COLOR, PKEY_MIN_VALUE, PKEY_MAX_VALUE, PKEY_TICK_SIZE,
                                   PKEY_POS, PKEY_SIZE, PKEY_ANCHOR, PKEY_BOUNDS, nullptr};
        concatArrays(keys, cGroupFigure::getAllowedPropertyKeys(), localKeys);
    }
    return keys;
}

void ThermometerFigure::addChildren()
{
    mercuryFigure = new cPathFigure("mercury");
    containerFigure = new cPathFigure("container");
    labelFigure = new cTextFigure("label");

    mercuryFigure->setFilled(true);
    mercuryFigure->setFillColor(RED);
    mercuryFigure->setLineOpacity(0);

    labelFigure->setAnchor(cFigure::ANCHOR_N);

    addFigure(mercuryFigure);
    addFigure(containerFigure);
    addFigure(labelFigure);
}

void ThermometerFigure::setValue(int series, simtime_t timestamp, double newValue)
{
    ASSERT(series == 0);
    // Note: we currently ignore timestamp
    if (value != newValue) {
        value = newValue;
        refresh();
    }
}

void ThermometerFigure::setTickGeometry(cLineFigure *tick, int index)
{
    if(numTicks - 1 == 0)
        return;

    double x, y, width, height, offset;
    getContainerGeometry(x, y, width, height, offset);

    double lineWidth = getBounds().height * CONTAINER_LINE_WIDTH_PERCENT/2;
    x += width + lineWidth;

    tick->setStart(Point(x, y + offset + height * index/(numTicks - 1)));
    tick->setEnd(Point(x + getBounds().width * TICK_LENGTH_PERCENT, y + offset + height * index/(numTicks - 1)));
    tick->setLineWidth(lineWidth);

}

void ThermometerFigure::getContainerGeometry(double &x, double &y, double &width, double &height, double &offset)
{
    x = getBounds().x + getBounds().width * CONTAINER_POS_PERCENT;
    y = getBounds().y + getBounds().width * CONTAINER_POS_PERCENT;
    width = getBounds().width * CONTAINER_WIDTH_PERCENT;
    offset = getBounds().height * CONTAINER_OFFSET_PERCENT;
    height = getBounds().height - width * (1 + 2*CONTAINER_POS_PERCENT) - getBounds().width*CONTAINER_POS_PERCENT - offset;
}

void ThermometerFigure::setNumberGeometry(cTextFigure *number, int index)
{
    if(numTicks - 1 == 0)
        return;

    double x, y, width, height, offset;
    getContainerGeometry(x, y, width, height, offset);

    double lineWidth = getBounds().height * CONTAINER_LINE_WIDTH_PERCENT;
    x += width + lineWidth + getBounds().width * TICK_LENGTH_PERCENT;
    y += offset + height * (numTicks - index - 1)/(numTicks - 1);

    size_t maxLength = std::to_string(int(max)).size();
    double pointSize = (getBounds().x + getBounds().width - x) / (maxLength / 2.0);

    number->setPosition(Point(x, y));
    number->setFont(cFigure::Font("", pointSize, 0));
}

void ThermometerFigure::setMercuryAndContainerGeometry()
{
    containerFigure->clearPath();
    mercuryFigure->clearPath();

    containerFigure->setLineWidth(getBounds().width * CONTAINER_LINE_WIDTH_PERCENT);

    double x, y, width, height, offset;
    getContainerGeometry(x, y, width, height, offset);

    containerFigure->addMoveTo(x, y);
    containerFigure->addLineRel(0, height + offset);
    //TODO this does not work with Qtenv:
    //containerFigure->addCubicBezierCurveRel(0, width, width, width, width, 0);
    containerFigure->addArcRel(width/2, width/2, 0, true, false, width, 0);
    containerFigure->addLineRel(0, -height - offset);
    containerFigure->addArcRel(width/2, width/2, 0, true, false, -width, 0);

    double mercuryLevel;
    double overflow = 0;
    if(std::isnan(value) || value < min)
        return;
    else if(value > max)
    {
        mercuryLevel = 1;
        // value < max so the mercury will be overflow
        overflow = offset;
        offset = 0;
    }
    else
        mercuryLevel = (value - min)/(max - min);

    mercuryFigure->addMoveTo(x, y + offset + height * (1 - mercuryLevel));
    mercuryFigure->addLineRel(0, height * mercuryLevel + overflow);
    //TODO this does not work with Qtenv:
    //mercuryFigure->addCubicBezierCurveRel(0, width, width, width, width, 0);
    mercuryFigure->addArcRel(width/2, width/2, 0, true, false, width, 0);
    mercuryFigure->addLineRel(0, -height * mercuryLevel - overflow);
    if (overflow > 0)
        mercuryFigure->addArcRel(width/2, width/2, 0, true, false, -width, 0);
    mercuryFigure->addClosePath();
}

void ThermometerFigure::redrawTicks()
{
    ASSERT(tickFigures.size() == numberFigures.size());

    int prevNumTicks = numTicks;
    numTicks = std::max(0.0, std::abs(max - min) / tickSize + 1);

    // Allocate ticks and numbers if needed
    if(numTicks > tickFigures.size())
        while(numTicks > tickFigures.size())
        {
            cLineFigure *tick = new cLineFigure();
            cTextFigure *number = new cTextFigure();

            number->setAnchor(cFigure::ANCHOR_W);

            tickFigures.push_back(tick);
            numberFigures.push_back(number);
        }

    // Add or remove figures from canvas according to previous number of ticks
    for(int i = numTicks; i < prevNumTicks; ++i)
    {
        removeFigure(tickFigures[i]);
        removeFigure(numberFigures[i]);
    }
    for(int i = prevNumTicks; i < numTicks; ++i)
    {
        addFigure(tickFigures[i]);
        addFigure(numberFigures[i]);
    }

    for(int i = 0; i < numTicks; ++i)
    {
        setTickGeometry(tickFigures[i], i);

        char buf[32];
        sprintf(buf, "%g", min + i*tickSize);
        numberFigures[i]->setText(buf);
        setNumberGeometry(numberFigures[i], i);
     }
}

void ThermometerFigure::layout()
{
    setMercuryAndContainerGeometry();

    for(int i = 0; i < numTicks; ++i)
    {
        setTickGeometry(tickFigures[i], i);
        setNumberGeometry(numberFigures[i], i);
    }

    labelFigure->setPosition(Point(getBounds().getCenter().x, getBounds().y + getBounds().height));
}

void ThermometerFigure::refresh()
{
    setMercuryAndContainerGeometry();
}

#endif // omnetpp 5

// } // namespace inet