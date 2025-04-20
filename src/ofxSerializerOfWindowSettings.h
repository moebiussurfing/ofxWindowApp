//
// Copyright (c) 2017 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier: MIT
//
// ofxSerializerOfWindowSettings.h by @moebiussurfing
// A simplified version from ofxSerializer.h using ofWindowSettings methods only.

#pragma once

#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include <nlohmann/json.hpp>

//namespace ofxSerializerOfWindowSettings {//breaks iter cbegin value below
namespace glm {

template <typename T, glm::precision P>
inline void to_json(nlohmann::json & j, const glm::tvec2<T, P> & v) {
	j = { { "x", v.x }, { "y", v.y } };
}

template <typename T, glm::precision P>
inline void from_json(const nlohmann::json & j, glm::tvec2<T, P> & v) {
	v.x = j.value("x", typename glm::tvec2<T, P>::value_type(0));
	v.y = j.value("y", typename glm::tvec2<T, P>::value_type(0));
}

//template <typename T, glm::precision P>
//inline void to_json(nlohmann::json & j, const glm::tvec3<T, P> & v) {
//	j = { { "x", v.x }, { "y", v.y }, { "z", v.z } };
//}

//template <typename T, glm::precision P>
//inline void from_json(const nlohmann::json & j, glm::tvec3<T, P> & v) {
//	v.x = j.value("x", typename glm::tvec3<T, P>::value_type(0));
//	v.y = j.value("y", typename glm::tvec3<T, P>::value_type(0));
//	v.z = j.value("z", typename glm::tvec3<T, P>::value_type(0));
//}

//template <typename T, glm::precision P>
//inline void to_json(nlohmann::json & j, const glm::tvec4<T, P> & v) {
//	j = { { "x", v.x }, { "y", v.y }, { "z", v.z }, { "w", v.w } };
//}

};
// namespace glm
// //namespace ofxSerializerOfWindowSettings.h

namespace ofxSerializerOfWindowSettings {

inline void to_json(nlohmann::json & j, const ofVec2f & v) {
	to_json(j, toGlm(v));
}

inline void from_json(const nlohmann::json & j, ofVec2f & v) {
	glm::vec2 g;
	from_json(j, g);
	v = toOf(g);
}

//inline void to_json(nlohmann::json & j, const ofVec3f & v) {
//	to_json(j, toGlm(v));
//}
//
//inline void from_json(const nlohmann::json & j, ofVec3f & v) {
//	glm::vec3 g;
//	from_json(j, g);
//	v = toOf(g);
//}

// -----------------------------------------------------------------------------

#include "ofWindowSettings.h"

NLOHMANN_JSON_SERIALIZE_ENUM( ofWindowMode, {
    { OF_WINDOW, "OF_WINDOW" },
    { OF_FULLSCREEN, "OF_FULLSCREEN"},
    { OF_GAME_MODE, "OF_GAME_MODE"}
})

inline void to_json(nlohmann::json & j, const ofWindowSettings & v) {
	if (v.isPositionSet())
		j["position"] = v.getPosition();
	if (v.isSizeSet())
		j["size"] = { { "width", v.getWidth() }, { "height", v.getHeight() } };
	//if (!v.title.empty())
	//	j["title"] = v.title;
	j["window_mode"] = v.windowMode;
}

inline void from_json(const nlohmann::json & j, ofWindowSettings & v) {
	auto iter = j.cbegin();
	while (iter != j.cend()) {
		const auto & key = iter.key();
		const auto & value = iter.value();

		if (key == "position")
			v.setPosition(value);
		else if (key == "size")
			v.setSize(value.value("width", 100),
				value.value("height", 100));
		//else if (key == "title")
		//	v.title = ofToString(value);
		else if (key == "window_mode")
			v.windowMode = value;
		++iter;
	}
}

}; // namespace ofxSerializerOfWindowSettings
