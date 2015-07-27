#include "rf2plugin.h"

class ExamplePlugin : public rf2::plugin
{
	int foo_;

public:
	ExamplePlugin() : rf2::plugin("Example") {}
	virtual ~ExamplePlugin() {}

	void OnStartup(long version)
	{
		foo_ = ReadConfig<int>("section1", "foo", 100);
	}
};

const char* Rf2PluginName() { return "ExamplePlugin"; }
rf2::plugin* Rf2PluginCreate() { return new ExamplePlugin(); }
