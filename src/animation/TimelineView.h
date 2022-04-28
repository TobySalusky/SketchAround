//
// Created by Tobiathan on 4/27/22.
//

#ifndef SENIORRESEARCH_TIMELINEVIEW_H
#define SENIORRESEARCH_TIMELINEVIEW_H


struct TimelineView {
	float startTime;
	float endTime;

	[[nodiscard]] float GetSpannedTime() const {
		return endTime - startTime;
	}
};


#endif //SENIORRESEARCH_TIMELINEVIEW_H
