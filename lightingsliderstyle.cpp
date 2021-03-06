#include <QDebug>
#include <qdrawutil.h>
#include <QStyleOptionComplex>

#include "lightingsliderstyle.h"

QRect LightingSliderStyle::subControlRect(ComplexControl control,
					  const QStyleOptionComplex *option,
					  SubControl subControl,
					  const QWidget *widget) const
{
	QRect rect;

	rect = QCommonStyle::subControlRect(control, option, subControl, widget);

	if (control == CC_Slider && subControl == SC_SliderHandle)
	{
		// this is the exact pixel dimensions of the handle png files
		rect.setWidth(21);
		rect.setHeight(21);
	}
	else if (control == CC_Slider && subControl == SC_SliderGroove)
	{
		// this is the exact pixel dimensions of the slider png files
		rect.setWidth(widget->width());
		rect.setHeight(widget->height());
	}

	return rect;
}

void LightingSliderStyle::drawComplexControl(QStyle::ComplexControl control,
					     const QStyleOptionComplex *option,
					     QPainter *painter,
					     const QWidget *widget) const
{
	if (control == CC_Slider)
	{
		if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(option))
		{
			QRect groove = subControlRect(CC_Slider, slider, SC_SliderGroove, widget);
			QRect handle = subControlRect(CC_Slider, slider, SC_SliderHandle, widget);

			if ((slider->subControls & SC_SliderGroove) && groove.isValid())
			{
				Qt::BGMode oldMode = painter->backgroundMode();
				painter->setBackgroundMode(Qt::TransparentMode);
				painter->drawPixmap(groove, groovePixmap);
				painter->setBackgroundMode(oldMode);
			}

			if ((slider->subControls & SC_SliderHandle) && handle.isValid())
			{
				Qt::BGMode oldMode = painter->backgroundMode();
				painter->setBackgroundMode(Qt::TransparentMode);
				painter->drawPixmap(handle, handlePixmap);
				painter->setBackgroundMode(oldMode);
			}
		}
	}
	else
	{
		QProxyStyle::drawComplexControl(control, option, painter, widget);
	}
}

int LightingSliderStyle::styleHint(QStyle::StyleHint hint,
				   const QStyleOption *option = 0,
				   const QWidget *widget = 0,
				   QStyleHintReturn *returnData = 0) const
{
	if (hint == QStyle::SH_Slider_AbsoluteSetButtons)
	{
		return (Qt::LeftButton | Qt::MidButton);
	}

	return QProxyStyle::styleHint(hint, option, widget, returnData);
}

void LightingSliderStyle::setColor(QColor ambientColor)
{
	QImage alphaMask(grooveMaskPixmap.toImage());

	QImage bg = QPixmap(QString::fromUtf8(":/main/graphics/AlphaSliderBG.png")).toImage();

	qreal r, g, b, alpha;
	r = ambientColor.redF();
	g = ambientColor.greenF();
	b = ambientColor.blueF();

	for (int x = 0; x < alphaMask.width(); x++)
	{
		for (int y = 0; y < alphaMask.height(); y++)
		{
			alpha = QColor(alphaMask.pixel(x, y)).alphaF();

			if (this->_blendWhite)
			{
				QColor backgroundColor = bg.pixel(x, y);

				// alpha blending equation
				// out = alpha * new + (1 - alpha) * old
				// since we are blending with white and white is 1.0, it drops out of the equation
				qreal rn, gn, bn;
				rn = alpha + (1.0f - alpha) * r;
				gn = alpha + (1.0f - alpha) * g;
				bn = alpha + (1.0f - alpha) * b;

				alphaMask.setColor(alphaMask.pixelIndex(x, y), qRgba(rn * 255, gn * 255, bn * 255, backgroundColor.alphaF() * 255));
			}
			else
			{
				alphaMask.setColor(alphaMask.pixelIndex(x, y), qRgba(r * 255, g * 255, b * 255, alpha * 255));
			}
		}
	}

	groovePixmap = QPixmap::fromImage(alphaMask);
}
