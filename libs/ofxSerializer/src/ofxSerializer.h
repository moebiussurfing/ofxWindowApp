//
// Copyright (c) 2017 Christopher Baker <https://christopherbaker.net>
//
// SPDX-License-Identifier: MIT
//


#ifndef OF_SERIALIZER_H
#define OF_SERIALIZER_H


#include "json.hpp"


// -----------------------------------------------------------------------------


#include "ofConstants.h"


NLOHMANN_JSON_SERIALIZE_ENUM( ofTargetPlatform, {
    { OF_TARGET_OSX, "OF_TARGET_OSX" },
    { OF_TARGET_MINGW, "OF_TARGET_MINGW"},
    { OF_TARGET_WINVS, "OF_TARGET_WINVS"},
    { OF_TARGET_IOS, "OF_TARGET_IOS" },
    { OF_TARGET_ANDROID, "OF_TARGET_ANDROID"},
    { OF_TARGET_LINUX, "OF_TARGET_LINUX"},
    { OF_TARGET_LINUX64, "OF_TARGET_LINUX64" },
    { OF_TARGET_LINUXARMV6L, "OF_TARGET_LINUXARMV6L"},
    { OF_TARGET_LINUXARMV7L, "OF_TARGET_LINUXARMV7L"},
    { OF_TARGET_EMSCRIPTEN, "OF_TARGET_EMSCRIPTEN" }
})


// -----------------------------------------------------------------------------


#include "ofVectorMath.h"
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"


namespace glm
{


template<typename T, glm::precision P>
inline void to_json(nlohmann::json& j, const glm::tvec2<T, P>& v)
{
    j = { { "x", v.x }, { "y", v.y } };
}


template<typename T, glm::precision P>
inline void from_json(const nlohmann::json& j, glm::tvec2<T, P>& v)
{
    v.x = j.value("x", typename glm::tvec2<T, P>::value_type(0));
    v.y = j.value("y", typename glm::tvec2<T, P>::value_type(0));
}


template<typename T, glm::precision P>
inline void to_json(nlohmann::json& j, const glm::tvec3<T, P>& v)
{
    j = { { "x", v.x }, { "y", v.y }, { "z", v.z } };
}


template<typename T, glm::precision P>
inline void from_json(const nlohmann::json& j, glm::tvec3<T, P>& v)
{
    v.x = j.value("x", typename glm::tvec3<T, P>::value_type(0));
    v.y = j.value("y", typename glm::tvec3<T, P>::value_type(0));
    v.z = j.value("z", typename glm::tvec3<T, P>::value_type(0));
}


template<typename T, glm::precision P>
inline void to_json(nlohmann::json& j, const glm::tvec4<T, P>& v)
{
    j = { { "x", v.x }, { "y", v.y }, { "z", v.z }, { "w", v.w } };
}


template<typename T, glm::precision P>
inline void from_json(const nlohmann::json& j, glm::tvec4<T, P>& v)
{
    v.x = j.value("x", typename glm::tvec3<T, P>::value_type(0));
    v.y = j.value("y", typename glm::tvec3<T, P>::value_type(0));
    v.z = j.value("z", typename glm::tvec3<T, P>::value_type(0));
    v.w = j.value("w", typename glm::tvec3<T, P>::value_type(1));
}


template<typename T, glm::precision P>
inline void to_json(nlohmann::json& j, const glm::tmat3x3<T, P>& v)
{
    j = { v[0], v[1], v[2] };
}


template<typename T, glm::precision P>
inline void from_json(const nlohmann::json& j, glm::tmat3x3<T, P>& v)
{
    v[0] = j[0];
    v[1] = j[1];
    v[2] = j[2];
}


template<typename T, glm::precision P>
inline void to_json(nlohmann::json& j, const glm::tmat4x4<T, P>& v)
{
    j = { v[0], v[1], v[2], v[3] };
}


template<typename T, glm::precision P>
inline void from_json(const nlohmann::json& j, glm::tmat4x4<T, P>& v)
{
    v[0] = j[0];
    v[1] = j[1];
    v[2] = j[2];
    v[3] = j[3];
}


template<typename T, glm::precision P>
inline void to_json(nlohmann::json& j, const glm::tquat<T, P>& v)
{
    j = { { "x", v.x }, { "y", v.y }, { "z", v.z }, { "w", v.w } };
}


template<typename T, glm::precision P>
inline void from_json(const nlohmann::json& j, glm::tquat<T, P>& v)
{
    v.x = j.value("x", typename glm::tquat<T, P>::value_type(1));
    v.y = j.value("y", typename glm::tquat<T, P>::value_type(0));
    v.z = j.value("z", typename glm::tquat<T, P>::value_type(0));
    v.w = j.value("w", typename glm::tquat<T, P>::value_type(0));
}


}; // namespace glm


inline void to_json(nlohmann::json& j, const ofVec2f& v)
{
    to_json(j, toGlm(v));
}


inline void from_json(const nlohmann::json& j, ofVec2f& v)
{
    glm::vec2 g;
    from_json(j, g);
    v = toOf(g);
}


inline void to_json(nlohmann::json& j, const ofVec3f& v)
{
    to_json(j, toGlm(v));
}


inline void from_json(const nlohmann::json& j, ofVec3f& v)
{
    glm::vec3 g;
    from_json(j, g);
    v = toOf(g);
}


inline void to_json(nlohmann::json& j, const ofVec4f& v)
{
    to_json(j, toGlm(v));
}


inline void from_json(const nlohmann::json& j, ofVec4f& v)
{
    glm::vec4 g;
    from_json(j, g);
    v = toOf(g);
}


inline void to_json(nlohmann::json& j, const ofMatrix3x3& v)
{
    to_json(j, toGlm(v));
}


inline void from_json(const nlohmann::json& j, ofMatrix3x3& v)
{
    glm::mat3 g;
    from_json(j, g);
    v = toOf(g);
}


inline void to_json(nlohmann::json& j, const ofMatrix4x4& v)
{
    to_json(j, toGlm(v));
}


inline void from_json(const nlohmann::json& j, ofMatrix4x4& v)
{
    glm::mat4 g;
    from_json(j, g);
    v = toOf(g);
}


inline void to_json(nlohmann::json& j, const ofQuaternion& v)
{
    to_json(j, toGlm(v));
}


inline void from_json(const nlohmann::json& j, ofQuaternion& v)
{
    glm::quat g;
    from_json(j, g);
    v = ofQuaternion(g);
}

// -----------------------------------------------------------------------------


#include "ofRectangle.h"


NLOHMANN_JSON_SERIALIZE_ENUM( ofAspectRatioMode, {
    { OF_ASPECT_RATIO_IGNORE, "OF_ASPECT_RATIO_IGNORE" },
    { OF_ASPECT_RATIO_KEEP, "OF_ASPECT_RATIO_KEEP"},
    { OF_ASPECT_RATIO_KEEP_BY_EXPANDING, "OF_ASPECT_RATIO_KEEP_BY_EXPANDING"}
})


NLOHMANN_JSON_SERIALIZE_ENUM( ofAlignVert, {
    { OF_ALIGN_VERT_IGNORE, "OF_ALIGN_VERT_IGNORE" },
    { OF_ALIGN_VERT_TOP, "OF_ALIGN_VERT_TOP"},
    { OF_ALIGN_VERT_BOTTOM, "OF_ALIGN_VERT_BOTTOM"},
    { OF_ALIGN_VERT_CENTER, "OF_ALIGN_VERT_CENTER"}
})


NLOHMANN_JSON_SERIALIZE_ENUM( ofAlignHorz, {
    { OF_ALIGN_HORZ_IGNORE, "OF_ALIGN_HORZ_IGNORE" },
    { OF_ALIGN_HORZ_LEFT, "OF_ALIGN_HORZ_LEFT"},
    { OF_ALIGN_HORZ_RIGHT, "OF_ALIGN_HORZ_RIGHT"},
    { OF_ALIGN_HORZ_CENTER, "OF_ALIGN_HORZ_CENTER"}
})


NLOHMANN_JSON_SERIALIZE_ENUM( ofScaleMode, {
    { OF_SCALEMODE_FIT, "OF_SCALEMODE_FIT" },
    { OF_SCALEMODE_FILL, "OF_SCALEMODE_FILL"},
    { OF_SCALEMODE_CENTER, "OF_SCALEMODE_CENTER"},
    { OF_SCALEMODE_STRETCH_TO_FILL, "OF_SCALEMODE_STRETCH_TO_FILL"}
})


inline void to_json(nlohmann::json& j, const ofRectangle& v)
{
    j = { { "x", v.x }, { "y", v.y }, { "width", v.width }, { "height", v.height } };
}


inline void from_json(const nlohmann::json& j, ofRectangle& v)
{
    v.x = j.value("x", float(0.0f));
    v.y = j.value("y", float(0.0f));
    v.width = j.value("width", float(0.0f));
    v.height = j.value("height", float(0.0f));
}

// -----------------------------------------------------------------------------


#include "ofColor.h"


template<typename PixelType>
inline void to_json(nlohmann::json& j, const ofColor_<PixelType>& p)
{
    j = { { "r", p.r }, { "g", p.g }, { "b", p.b }, { "a", p.a } };
}


template<typename PixelType>
inline void from_json(const nlohmann::json& j, ofColor_<PixelType>& p)
{
    p.r = j.value("r", ofColor_<PixelType>::limit());
    p.g = j.value("g", ofColor_<PixelType>::limit());
    p.b = j.value("b", ofColor_<PixelType>::limit());
    p.a = j.value("a", ofColor_<PixelType>::limit());
}


// -----------------------------------------------------------------------------


#include "ofMesh.h"


#ifndef TARGET_OPENGLES

NLOHMANN_JSON_SERIALIZE_ENUM( ofPrimitiveMode, {
    { OF_PRIMITIVE_TRIANGLES, "OF_PRIMITIVE_TRIANGLES" },
    { OF_PRIMITIVE_TRIANGLE_STRIP, "OF_PRIMITIVE_TRIANGLE_STRIP"},
    { OF_PRIMITIVE_TRIANGLE_FAN, "OF_PRIMITIVE_TRIANGLE_FAN"},
    { OF_PRIMITIVE_LINES, "OF_PRIMITIVE_LINES" },
    { OF_PRIMITIVE_LINE_STRIP, "OF_PRIMITIVE_LINE_STRIP"},
    { OF_PRIMITIVE_LINE_LOOP, "OF_PRIMITIVE_LINE_LOOP"},
    { OF_PRIMITIVE_POINTS, "OF_PRIMITIVE_POINTS"},

    { OF_PRIMITIVE_LINES_ADJACENCY, "OF_PRIMITIVE_LINES_ADJACENCY"},
    { OF_PRIMITIVE_LINE_STRIP_ADJACENCY, "OF_PRIMITIVE_LINE_STRIP_ADJACENCY"},
    { OF_PRIMITIVE_TRIANGLES_ADJACENCY, "OF_PRIMITIVE_TRIANGLES_ADJACENCY"},
    { OF_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY, "OF_PRIMITIVE_TRIANGLE_STRIP_ADJACENCY"},
    { OF_PRIMITIVE_PATCHES, "OF_PRIMITIVE_PATCHES"}
})

#else

NLOHMANN_JSON_SERIALIZE_ENUM( ofPrimitiveMode, {
    { OF_PRIMITIVE_TRIANGLES, "OF_PRIMITIVE_TRIANGLES" },
    { OF_PRIMITIVE_TRIANGLE_STRIP, "OF_PRIMITIVE_TRIANGLE_STRIP"},
    { OF_PRIMITIVE_TRIANGLE_FAN, "OF_PRIMITIVE_TRIANGLE_FAN"},
    { OF_PRIMITIVE_LINES, "OF_PRIMITIVE_LINES" },
    { OF_PRIMITIVE_LINE_STRIP, "OF_PRIMITIVE_LINE_STRIP"},
    { OF_PRIMITIVE_LINE_LOOP, "OF_PRIMITIVE_LINE_LOOP"},
    { OF_PRIMITIVE_POINTS, "OF_PRIMITIVE_POINTS"},
})

#endif


//std::vector<V> vertices;
//std::vector<C> colors;
//std::vector<N> normals;
//std::vector<T> texCoords;
//std::vector<ofIndexType> indices;
//
//// this variables are only caches and returned always as const
//// mutable allows to change them from const methods
//mutable std::vector<ofMeshFace_<V,N,C,T>> faces;
//mutable bool bFacesDirty;
//
//bool bVertsChanged, bColorsChanged, bNormalsChanged, bTexCoordsChanged,
//bIndicesChanged;
//ofPrimitiveMode mode;
//
//bool useColors;
//bool useTextures;
//bool useNormals;
//bool useIndices;



template<class V, class N, class C, class T>
inline void to_json(nlohmann::json& j, const ofMesh_<V, N, C, T>& v)
{
    j["vertices"] = v.getVertices();
    j["normals"] = v.getNormals();
    j["colors"] = v.getColors();
    j["tex_coords"] = v.getTexCoords();

    j["indices"] = v.getIndices();

    j["using_normals"] = v.usingNormals();
    j["using_colors"] = v.usingColors();
    j["using_textures"] = v.usingTextures();

    j["using_indices"] = v.usingIndices();

    j["primitive_mode"] = v.getMode();
}


template<class V, class N, class C, class T>
inline void from_json(const nlohmann::json& j, ofMesh_<V, N, C, T>& v)
{
    v = ofMesh_<V, N, C, T>();
    v.addVertices(j.value("vertices", std::vector<V>()));
    v.addNormals(j.value("normals", std::vector<N>()));
    v.addColors(j.value("colors", std::vector<C>()));
    v.addTexCoords(j.value("tex_coords", std::vector<T>()));

    v.addIndices(j.value("indices", std::vector<ofIndexType>()));

    v.setMode(j.value("primitive_mode", OF_PRIMITIVE_TRIANGLES));

    // TODO
    bool usingColors = j.value("using_colors", true);
    bool usingTextures = j.value("using_textures", true);
    bool usingNormals = j.value("using_normals", true);
    bool usingIndices = j.value("using_indices", true);

    if (j.value("using_colors", true)) v.enableColors();
    else v.disableColors();

    if (j.value("using_textures", true)) v.enableTextures();
    else v.disableTextures();

    if (j.value("using_normals", true)) v.enableTextures();
    else v.disableTextures();

    if (j.value("using_indices", true)) v.enableIndices();
    else v.disableIndices();

}



// -----------------------------------------------------------------------------


#include "ofPolyline.h"


template<typename VertexType>
inline void to_json(nlohmann::json& j, const ofPolyline_<VertexType>& v)
{
    j["is_closed"] = v.isClosed();
    nlohmann::json vertices = nlohmann::json::array();
    for (auto& vertex: v)
        vertices.push_back(vertex);
    j["vertices"] = vertices;
}


template<typename VertexType>
inline void from_json(const nlohmann::json& j, ofPolyline_<VertexType>& v)
{
    const auto& vertices = j["vertices"];
    for (auto& vertex: vertices)
        v.addVertex(vertex.get<VertexType>());
    v.setClosed(j.value("is_closed", false));
}


// -----------------------------------------------------------------------------


#include "ofPath.h"


NLOHMANN_JSON_SERIALIZE_ENUM( ofPath::Command::Type, {
    { ofPath::Command::Type::moveTo, "MOVE_TO" },
    { ofPath::Command::Type::lineTo, "LINE_TO"},
    { ofPath::Command::Type::curveTo, "CURVE_TO"},
    { ofPath::Command::Type::bezierTo, "BEZIER_TO" },
    { ofPath::Command::Type::quadBezierTo, "QUAD_BEZIER_TO"},
    { ofPath::Command::Type::arc, "ARC"},
    { ofPath::Command::Type::arcNegative, "ARC_NEGATIVE" },
    { ofPath::Command::Type::close, "CLOSE"}
})


template<typename VertexType>
inline void to_json(nlohmann::json& j, const ofPath::Command& v)
{
    j["type"] = v.type;
    j["to"] = v.to;
    j["cp_1"] = v.to;
    j["cp_2"] = v.to;
    j["radius_x"] = v.radiusX;
    j["radius_y"] = v.radiusY;
    j["angle_begin"] = v.angleBegin;
    j["angle_end"] = v.angleEnd;
}


template<typename VertexType>
inline void from_json(const nlohmann::json& j, ofPath::Command& v)
{
    // If there isn't a type member, then we want it to throw an exception.
    ofPath::Command::Type type = j["type"];

    switch (type)
    {
        case ofPath::Command::Type::close:
            v = ofPath::Command(type);
            return;
        case ofPath::Command::Type::moveTo:
        case ofPath::Command::Type::lineTo:
        case ofPath::Command::Type::curveTo:
            v = ofPath::Command(type, j["to"]);
            return;
        case ofPath::Command::Type::bezierTo:
        case ofPath::Command::Type::quadBezierTo:
            v = ofPath::Command(type, j["to"], j["cp_1"], j["cp_2"]);
            return;
        case ofPath::Command::Type::arc:
        case ofPath::Command::Type::arcNegative:
            v = ofPath::Command(type,
                                j["to"],
                                j["radius_x"],
                                j["radius_y"],
                                j["angle_begin"],
                                j["angle_end"]);
            return;
    }
}


// -----------------------------------------------------------------------------


#include "ofLog.h"


NLOHMANN_JSON_SERIALIZE_ENUM( ofLogLevel, {
    { OF_LOG_VERBOSE, "OF_LOG_VERBOSE" },
    { OF_LOG_NOTICE, "OF_LOG_NOTICE"},
    { OF_LOG_WARNING, "OF_LOG_WARNING"},
    { OF_LOG_ERROR, "OF_LOG_ERROR" },
    { OF_LOG_FATAL_ERROR, "OF_LOG_FATAL_ERROR"},
    { OF_LOG_SILENT, "OF_LOG_SILENT"}
})


// -----------------------------------------------------------------------------


#include "ofWindowSettings.h"


NLOHMANN_JSON_SERIALIZE_ENUM( ofWindowMode, {
    { OF_WINDOW, "OF_WINDOW" },
    { OF_FULLSCREEN, "OF_FULLSCREEN"},
    { OF_GAME_MODE, "OF_GAME_MODE"}
})


inline void to_json(nlohmann::json& j, const ofWindowSettings& v)
{
    if (v.isPositionSet())
        j["position"] = v.getPosition();
    if (v.isSizeSet())
        j["size"] = { { "width", v.getWidth() }, { "height", v.getHeight() } };;

    if (!v.title.empty())
        j["title"] = v.title;

    j["window_mode"] = v.windowMode;
}


inline void from_json(const nlohmann::json& j, ofWindowSettings& v)
{
    auto iter = j.cbegin();
    while (iter != j.cend())
    {
        const auto& key = iter.key();
        const auto& value = iter.value();

        if (key == "position") v.setPosition(value);
        else if (key == "size")
            v.setSize(value.value("width", 100),
                      value.value("height", 100));
        else if (key == "title") v.title = value;
        else if (key == "window_mode") v.windowMode = value;
        ++iter;
    }
}


// -----------------------------------------------------------------------------


#include "ofVideoBaseTypes.h"


NLOHMANN_JSON_SERIALIZE_ENUM( ofLoopType, {
    { OF_LOOP_NONE, "OF_LOOP_NONE" },
    { OF_LOOP_PALINDROME, "OF_LOOP_PALINDROME"},
    { OF_LOOP_NORMAL, "OF_LOOP_NORMAL"}
})


///// \brief A structure describing attributes of a video format.
/////
///// An ofVideoFormat is used to describe the size, pixel format and frame rates
///// offered by a video device.
/////
///// \sa ofVideoDevice
//class ofVideoFormat{
//public:
//	/// \brief The pixel format of the video format.
//	ofPixelFormat pixelFormat;

//	/// \brief The width of the video format in pixels.
//	int width;

//	/// \brief The height of the video format in pixels.
//	int height;

//	/// \brief A list of framerates for this video format in frames per second.
//	std::vector<float> framerates;
//};

///// \brief A structure describing attributes of a video device.
/////
///// An ofVideoDevice can represent a camera, grabber or other frame source.
//class ofVideoDevice{
//public:
//	/// \brief The video device ID.
//	int id;

//	/// \brief The video device name.
//	std::string deviceName;

//	/// \brief The video device hardware name.
//	std::string hardwareName;

//	/// \brief Unique identifier for the device if it has one.
//	std::string serialID;

//	/// \brief A list of video device formats provided by the device.
//	/// \sa ofVideoFormat
//	std::vector<ofVideoFormat> formats;

//	/// \brief Is true if this video device is available.
//	bool bAvailable;
//};


// -----------------------------------------------------------------------------


#include "ofSoundBaseTypes.h"


NLOHMANN_JSON_SERIALIZE_ENUM( ofSoundDevice::Api, {
    { ofSoundDevice::Api::UNSPECIFIED, "UNSPECIFIED" },
    { ofSoundDevice::Api::DEFAULT, "DEFAULT" },
    { ofSoundDevice::Api::ALSA, "ALSA" },
    { ofSoundDevice::Api::PULSE, "PULSE" },
    { ofSoundDevice::Api::OSS, "OSS" },
    { ofSoundDevice::Api::JACK, "JACK" },
    { ofSoundDevice::Api::OSX_CORE, "OSX_CORE" },
    { ofSoundDevice::Api::MS_WASAPI, "MS_WASAPI" },
    { ofSoundDevice::Api::MS_ASIO, "MS_ASIO" },
    { ofSoundDevice::Api::MS_DS, "MS_DS" }
})


//class ofSoundDevice {
//public:
//    enum Api {
//        UNSPECIFIED,
//        DEFAULT,
//        ALSA,     /*!< The Advanced Linux Sound Architecture API. */
//        PULSE,    /*!< The Linux PulseAudio API. */
//        OSS,      /*!< The Linux Open Sound System API. */
//        JACK,      /*!< The Jack Low-Latency Audio Server API. */
//        OSX_CORE,    /*!< Macintosh OS-X Core Audio API. */
//        MS_WASAPI, /*!< The Microsoft WASAPI API. */
//        MS_ASIO,   /*!< The Steinberg Audio Stream I/O API. */
//        MS_DS,     /*!< The Microsoft Direct Sound API. */
//        NUM_APIS
//    } api = UNSPECIFIED;

//    friend std::ostream& operator << (std::ostream& os, const ofSoundDevice& dev);
//    friend std::ostream& operator << (std::ostream& os, const std::vector<ofSoundDevice>& devs);

//    /// \brief Descriptive name for the device
//    /// This is the same string that ofSoundStream::getMatchingDevices() will be looking for
//    std::string name{"Unknown"};

//    /// \brief The device's unique ID (to be used in ofSoundStream::setDeviceID() )
//    int deviceID = -1;

//    /// \brief Number of input channels the device supports
//    unsigned int inputChannels = 0;

//    /// \brief Number of output channels the device supports
//    unsigned int outputChannels = 0;

//    /// \brief If true, this device will be used by ofSoundStream unless changed with setDeviceID()
//    bool isDefaultInput = false;

//    /// \brief If true, this device will be used by ofSoundStream unless changed with setDeviceID()
//    bool isDefaultOutput = false;

//    /// \brief List of sample rates the device claims to support
//    std::vector<unsigned int> sampleRates;
//};

//class ofSoundStreamSettings {
//public:
//	virtual ~ofSoundStreamSettings() {}
//	size_t sampleRate = 44100;
//	size_t bufferSize = 256;
//	size_t numBuffers = 4;
//	size_t numInputChannels = 0;
//	size_t numOutputChannels = 0;
//	virtual bool setInDevice(const ofSoundDevice & device);
//	virtual bool setOutDevice(const ofSoundDevice & device);
//	virtual bool setApi(ofSoundDevice::Api api);
//	virtual const ofSoundDevice * getInDevice() const;
//	virtual const ofSoundDevice * getOutDevice() const;
//	virtual ofSoundDevice::Api getApi() const;

//	template<typename Listener>
//	void setInListener(Listener * inListener){
//		inCallback = std::bind(static_cast<void(Listener::*)(ofSoundBuffer &)>(&Listener::audioIn), inListener, std::placeholders::_1);
//	}

//	template<typename Listener>
//	void setOutListener(Listener * outListener){
//		outCallback = std::bind(static_cast<void(Listener::*)(ofSoundBuffer &)>(&Listener::audioOut), outListener, std::placeholders::_1);
//	}

//	std::function<void(ofSoundBuffer &)> inCallback;
//	std::function<void(ofSoundBuffer &)> outCallback;
//private:
//	ofSoundDevice inDevice;
//	ofSoundDevice outDevice;
//	ofSoundDevice::Api api = ofSoundDevice::Api::UNSPECIFIED;
//};


#include "ofFbo.h"


inline void to_json(nlohmann::json& j, const ofFboSettings& v)
{
    j["size"]["width"] = v.width;
    j["size"]["height"] = v.height;
    j["num_color_buffers"] = v.numColorbuffers;
    j["color_formats"] = v.colorFormats;
    j["use_depth"] = v.useDepth;
    j["use_stencil"] = v.useStencil;
    j["depth_stencil_as_texture"] = v.depthStencilAsTexture;
    j["texture_target"] = v.textureTarget;
    j["internal_format"] = v.internalformat;
    j["depth_stencil_internal_format"] = v.depthStencilInternalFormat;
    j["wrap_mode_horizontal"] = v.wrapModeHorizontal;
    j["wrap_mode_vertical"] = v.wrapModeVertical;
    j["min_filter"] = v.minFilter;
    j["max_filter"] = v.maxFilter;
    j["num_samples"] = v.numSamples;
}


inline void from_json(const nlohmann::json& j, ofFboSettings& v)
{
    if (j.count("size"))
    {
        int w = j["size"].value("width", 0);
        int h = j["size"].value("height", 0);

        if (w > 0 && h > 0)
        {
            v.width = w;
            v.height = h;
        }
        else
        {
            ofLogWarning("from_json") << "Invalid width and/or height: " << w << ", " << h;
        }
    }

    v.numColorbuffers = j.value("num_color_buffers", 1);
    v.colorFormats = j.value("color_formats", std::vector<GLint>());
    v.useDepth = j.value("use_depth", false);
    v.useStencil = j.value("use_stencil", false);
    v.depthStencilAsTexture = j.value("depth_stencil_as_texture", false);
    v.textureTarget =
#ifndef TARGET_OPENGLES
    v.textureTarget = j.value("texture_target", ofGetUsingArbTex() ? GL_TEXTURE_RECTANGLE_ARB : GL_TEXTURE_2D);
#else
    v.textureTarget = j.value("texture_target", GL_TEXTURE_2D);
#endif
    v.internalformat = j.value("internal_format", GL_RGBA);
    v.depthStencilInternalFormat = j.value("depth_stencil_internal_format", GL_DEPTH_COMPONENT24);
    v.wrapModeHorizontal = j.value("wrap_mode_horizontal", GL_CLAMP_TO_EDGE);
    v.wrapModeVertical = j.value("wrap_mode_vertical", GL_CLAMP_TO_EDGE);
    v.minFilter = j.value("min_filter", GL_LINEAR);
    v.maxFilter = j.value("max_filter", GL_LINEAR);
    v.numSamples = j.value("num_samples", 0);
}


namespace ofx {
namespace Serializer {


inline void ApplyLoggingSettings(const nlohmann::json& settings)
{
    auto iter = settings.cbegin();
    while (iter != settings.cend())
    {
        const auto& key = iter.key();
        const auto& value = iter.value();

        if (key == "logger")
        {
            std::string type = value.value("type", "console");

            if (type == "console")
            {
                ofLogVerbose("LoadLoggingSettings") << "Console logger used.";
                ofLogToConsole();
            }
            else if (type == "file")
            {
                std::string filename = value.value("filename", "log.log");
                bool append = value.value("append", true);
                ofLogVerbose("LoadLoggingSettings") << "ofLogToFile(" << filename << ", " << append << ");";
                ofLogToFile(filename, append);
            }
            else
            {
                ofLogWarning("LoadLoggingSettings") << "Unknown logger type: " << type << ", using default console logger.";
            }
        }
        else if (key == "level")
        {
            ofLogVerbose("LoadLoggingSettings") << "ofSetLogLevel(" << value << ");";
            ofSetLogLevel(value);
        }
        else if (key == "modules")
        {
            auto moduleIter = value.cbegin();
            while (moduleIter != value.cend())
            {
                ofLogVerbose("LoadLoggingSettings") << "ofSetLogLevel(" << moduleIter.key() << ", " << moduleIter.value() << ");";
                ofSetLogLevel(moduleIter.key(),
                              moduleIter.value());
                ++moduleIter;
            }
        }
        else ofLogWarning("LoadLoggingSettings") << "Unknown key: " << key;

        ++iter;
    }
}


inline void ApplyWindowSettings(const nlohmann::json& settings)
{
    auto iter = settings.cbegin();
    while (iter != settings.cend())
    {
        const auto& key = iter.key();
        const auto& value = iter.value();

        if (key == "position")
        {
            glm::vec2 position = value;
            ofLogVerbose("LoadWindowSettings") << "ofSetWindowPosition(" << position.x << ", " << position.y << ");";
            ofSetWindowPosition(position.x, position.y);
        }
        else if (key == "size")
        {
            float width = value.value("width", 512);
            float height = value.value("height", 512);
            ofLogVerbose("LoadWindowSettings") << "ofSetWindowShape(" << width << ", " << height << ");";
            ofSetWindowShape(width, height);
        }
        else if (key == "title" && !value.empty())
        {
            ofLogVerbose("LoadWindowSettings") << "ofSetWindowTitle(" << value << ");";
            ofSetWindowTitle(value);
        }
        else if (key == "window_mode")
        {
            ofWindowMode mode = value;
            bool fullscreen = (mode == OF_FULLSCREEN || mode == OF_GAME_MODE);
            ofLogVerbose("LoadWindowSettings") << "ofSetVerticalSync(" << fullscreen << ");";
            ofSetFullscreen(fullscreen);
        }
        else if (key == "vertical_sync")
        {
            ofLogVerbose("LoadWindowSettings") << "ofSetVerticalSync(" << value << ");";
            ofSetVerticalSync(value);
        }
        else if (key == "frame_rate")
        {
            ofLogVerbose("LoadWindowSettings") << "ofSetFrameRate(" << value << ");";
            ofSetFrameRate(value);
        }
        else if (key == "hide_cursor")
        {
            bool b = value;

            if (b)
            {
                ofLogVerbose("LoadWindowSettings") << "ofHideCursor();";
                ofHideCursor();
            }
            else
            {
                ofLogVerbose("LoadWindowSettings") << "ofShowCursor();";
                ofShowCursor();
            }
        }
        else ofLogWarning("LoadWindowSettings") << "Unknown key: " << key;
        ++iter;
    }
}


/// \brief Will load settings saved in the "app" settings.
inline void ApplyAppSettings(const nlohmann::json& settings)
{
    if (settings.find("logging") != settings.end())
        ApplyLoggingSettings(settings["logging"]);

    if (settings.find("window") != settings.end())
        ApplyWindowSettings(settings["window"]);

    auto iter = settings.cbegin();
    while (iter != settings.cend())
    {
        const auto& key = iter.key();
        const auto& value = iter.value();

        // We enforce the ordering above.
        if (key == "logging") { }
        else if (key == "window") { }
        else ofLogWarning("LoadAppSettings") << "Unknown key: " << key;
        ++iter;
    }
}


} } // namespace ofx::Serializer


#endif // OF_SERIALIZER_H
