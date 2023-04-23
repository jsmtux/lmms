#ifndef I_TIME_LINE_WIDGET_H_
#define I_TIME_LINE_WIDGET_H_

#include <QDomElement>
#include <QDomDocument>
#include <QObject>

namespace lmms {

class TimePos;

namespace gui{

enum TimeLineBehaviourAtStopStates
{
    BackToZero,
    BackToStart,
    KeepStopPosition
} ;

enum TimeLineAutoScrollStates
{
    AutoScrollEnabled,
    AutoScrollDisabled
} ;

class ITimeLineWidget: public QObject {
    Q_OBJECT
public:
	virtual ~ITimeLineWidget() {}
    virtual void savePos( const TimePos & pos ) = 0;
    virtual const TimePos & loopBegin() const = 0;
    virtual const TimePos & loopEnd() const = 0;
    virtual TimeLineBehaviourAtStopStates behaviourAtStop() const = 0;
    virtual const TimePos & savedPos() const = 0;
    virtual void toggleLoopPoints( int _n ) = 0;
    virtual bool loopPointsEnabled() const = 0;
    virtual QDomElement saveState( QDomDocument & _doc, QDomElement & _parent ) = 0;
    virtual void restoreState( const QDomElement & _this ) = 0;
    virtual QString nodeName() const = 0;
    virtual void setPixelsPerBar( float ppb ) = 0;
    virtual int markerX( const TimePos & _t ) const = 0;
	virtual TimeLineAutoScrollStates autoScroll() const = 0;
    virtual void setFixedWidth(int _width) = 0;
signals:
	void positionMarkerMoved();
};

}
}

#endif // I_TIME_LINE_WIDGET_H_