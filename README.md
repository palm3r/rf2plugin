# rf2plugin

A simple class for easy development of rFactor2 plugins

## Features

* Read/Write configuration from/to INI file (rFactor2\Plugins\PLUGIN\PLUGIN.ini)
* Read/Write configuration from/to specified name INI file (rFactor2\Plugins\PLUGIN\NAME.ini)
* Identify whether running on in either of server or client.
* Send chat message to players

## Usage

1. Import following files to your project.

	* rf2plugin.h
	* InternalsPlugin.hpp
	* PluginObjects.hpp

1. Write your own plugin class derived from rf2::plugin class.

1. Implement following functions by yourself.

* const char* Rf2PluginName() { return "Your Plugin Name"; }
* rf2::plugin* Rf2PluginCreate() { return new YourPlugin(); }
