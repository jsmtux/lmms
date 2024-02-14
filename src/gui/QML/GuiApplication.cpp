#include "GuiApplication.h"


namespace lmms
{


namespace gui
{

IGuiApplication* getGUIInterface()
{
	return GuiApplication::instance();
}


std::unique_ptr<GuiApplication> GuiApplication::s_instance;

GuiApplication::GuiApplication() {}

GuiApplication* GuiApplication::instance()
{
	if (!s_instance) {
		s_instance = std::make_unique<GuiApplication>();
	}
	return s_instance.get();
}

BaseTrackModel* CreateTrackModel(ITrack* track, QObject* parent){

	switch(track->type()) {
		case ITrack::InstrumentTrack:
		{
			auto instrumentTrackSpecific = static_cast<IInstrumentTrack*>(track->getTrackTypeSpecificInterface());
			return new InstrumentTrackModel(instrumentTrackSpecific, track, parent);
		}
		case ITrack::PatternTrack:
		{
			auto patternTrackSpecific = static_cast<IPatternTrack*>(track->getTrackTypeSpecificInterface());
			return new PatternTrackModel(patternTrackSpecific, track, parent);
		}
		case ITrack::SampleTrack:
			return new BaseTrackModel(track, BaseTrackModel::TrackType::Sample, parent);
		case ITrack::AutomationTrack:
			return new BaseTrackModel(track, BaseTrackModel::TrackType::Automation, parent);
		default:
			return nullptr;
	}
}

BaseClipModel* CreateClipModel(IClip* clip, QObject* parent)
{
	switch(clip->getType()) {
		case lmms::ClipType::Midi:
			return new InstrumentClipModel(clip, parent);
		case lmms::ClipType::Pattern:
			return new PatternClipModel(clip, parent);
		case lmms::ClipType::Automation:
			return new BaseClipModel(clip, BaseClipModel::ClipType::Automation, parent);
		case lmms::ClipType::Sample:
			return new BaseClipModel(clip, BaseClipModel::ClipType::Sample, parent);
	}
}

}
}