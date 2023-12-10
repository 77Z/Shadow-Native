#ifndef SHADOW_NATIVE_ANALYTICS_HPP
#define SHADOW_NATIVE_ANALYTICS_HPP

namespace Shadow {

class AnalyticsBroker {
public:
	AnalyticsBroker();
	~AnalyticsBroker();

	// Auto destruction
	AnalyticsBroker(const AnalyticsBroker &) = delete;
	AnalyticsBroker &operator=(const AnalyticsBroker &) = delete;
};

}

#endif /* SHADOW_NATIVE_ANALYTICS_HPP */