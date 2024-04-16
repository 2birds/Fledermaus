#pragma once

#include "rapidjson/document.h"
#include <fstream>
#include <sstream>
#include <string>

#define CONFIG_FILE_NAME "fledermaus_config.json"

#define SPEED_NAME Speed
#define FIST_TO_LIFT_NAME FistToLiftActive
#define RIGHT_CLICK_ACTIVE_NAME RightClickActive
#define LOCK_MOUSE_ON_SCROLL_NAME LockMouseOnScroll
#define SCROLLING_ON_NAME ScrollingActive
#define SCROLLING_SPEED_NAME ScrollingSpeed
#define ORIENTATION_NAME VerticalOrientation

#define STRINGIFY(x) #x
#define STRINGIFY_HELPER(x) STRINGIFY(x)
#define TOKENPASTE_HELPER(x, y) x ## y
#define TOKENPASTE(x, y) TOKENPASTE_HELPER(x, y)

namespace rjs=rapidjson;


#define SETTERS_AND_GETTERS_BOOL(name, def) private: \
    bool TOKENPASTE(name, _) = def; \
    public: \
    void TOKENPASTE(Set, name)(const bool name) \
    { \
        TOKENPASTE(name, _) = name; \
    } \
    bool TOKENPASTE(Get, name)##() \
    { \
        return TOKENPASTE(name, _); \
    }

#define SETTERS_AND_GETTERS_FLOAT(name, default) private: \
    float TOKENPASTE(name, _) = default; \
    public: \
    void TOKENPASTE(Set, name)(const float name) \
    { \
        TOKENPASTE(name, _) = name; \
    } \
    float TOKENPASTE(Get, name) ##() \
    { \
        return TOKENPASTE(name, _); \
    }

class ConfigReader {

    SETTERS_AND_GETTERS_FLOAT(SPEED_NAME, 2.0f);
    SETTERS_AND_GETTERS_BOOL(SCROLLING_ON_NAME, true);
    SETTERS_AND_GETTERS_FLOAT(SCROLLING_SPEED_NAME, 2.0f);
    SETTERS_AND_GETTERS_BOOL(ORIENTATION_NAME, true);
    SETTERS_AND_GETTERS_BOOL(LOCK_MOUSE_ON_SCROLL_NAME, true);
    SETTERS_AND_GETTERS_BOOL(RIGHT_CLICK_ACTIVE_NAME, true);
    SETTERS_AND_GETTERS_BOOL(FIST_TO_LIFT_NAME, true);

    private:
    std::string config_file_name_;
    rjs::Document d_;

    public:
    ConfigReader(std::string config_file_name) :
    config_file_name_(config_file_name)
    {
        std::ifstream ifs(config_file_name_);

        if (!ifs.fail())
        {
            std::string line;
            std::stringstream ss;
            while (std::getline(ifs, line))
            {
                ss << line;
            }

            d_.Parse(ss.str().c_str());

            validateAndLoadJson(); 
        }
        else
        {
            printf("Couldn't find config file.\n");
        }
    }

    ConfigReader() : ConfigReader(CONFIG_FILE_NAME)
    {}

    ~ConfigReader() {}

    void print()
    {
        printf( STRINGIFY_HELPER(SPEED_NAME) ": %f\n", TOKENPASTE(SPEED_NAME, _));
        printf( STRINGIFY_HELPER(FIST_TO_LIFT_NAME) ": %s\n", TOKENPASTE(FIST_TO_LIFT_NAME, _) ? "true" : "false");
        printf( STRINGIFY_HELPER(RIGHT_CLICK_ACTIVE_NAME) ": %s\n", TOKENPASTE(RIGHT_CLICK_ACTIVE_NAME, _) ? "true" : "false");
        printf( STRINGIFY_HELPER(LOCK_MOUSE_ON_SCROLL_NAME) ": %s\n", TOKENPASTE(LOCK_MOUSE_ON_SCROLL_NAME, _) ? "true" : "false");
        printf( STRINGIFY_HELPER(SCROLLING_ON_NAME) ": %s\n", TOKENPASTE(SCROLLING_ON_NAME, _) ? "true" : "false");
        printf( STRINGIFY_HELPER(SCROLLING_SPEED_NAME) ": %f\n", TOKENPASTE(SCROLLING_SPEED_NAME, _));
        printf( STRINGIFY_HELPER(ORIENTATION_NAME) ": %s\n", TOKENPASTE(ORIENTATION_NAME, _) ? "true" : "false");
    }

    private:
    void validateAndLoadJson()
    {
        if (d_.HasMember(STRINGIFY_HELPER(ORIENTATION_NAME )))
        {
            // assert(d_[STRINGIFY_HELPER(ORIENTATION_NAME)].IsString());

            TOKENPASTE(ORIENTATION_NAME, _) = d_[STRINGIFY_HELPER(ORIENTATION_NAME)].GetBool();
        }
        else
        {
            printf(STRINGIFY_HELPER(ORIENTATION_NAME) " not found!\n");
        }
        
        if (d_.HasMember(STRINGIFY_HELPER(SPEED_NAME)))
        {
            // assert(d_[STRINGIFY(SPEED_NAME)].IsNumber());
            TOKENPASTE(SPEED_NAME, _) = d_[STRINGIFY_HELPER(SPEED_NAME)].GetFloat();
        }
        else
        {
            printf(STRINGIFY_HELPER(SPEED_NAME) " not found!\n");
        }
        
        if (d_.HasMember(STRINGIFY_HELPER(FIST_TO_LIFT_NAME)))
        {
            // assert(d_[STRINGIFY(SCROLLING_SPEED_NAME)].IsFloat());
            TOKENPASTE(FIST_TO_LIFT_NAME, _) = d_[STRINGIFY_HELPER(FIST_TO_LIFT_NAME)].GetBool();
        }
        else
        {
            printf(STRINGIFY_HELPER(FIST_TO_LIFT_NAME) " not found!\n");
        }

        if (d_.HasMember(STRINGIFY_HELPER(RIGHT_CLICK_ACTIVE_NAME)))
        {
            // assert(d_[STRINGIFY(RIGHT_CLICK_ACTIVE_NAME)].IsBool());
            TOKENPASTE(RIGHT_CLICK_ACTIVE_NAME, _) = d_[STRINGIFY_HELPER(RIGHT_CLICK_ACTIVE_NAME)].GetBool();
        }
        else
        {
            printf(STRINGIFY_HELPER(RIGHT_CLICK_ACTIVE_NAME) " not found!\n");
        }

        if (d_.HasMember(STRINGIFY_HELPER(LOCK_MOUSE_ON_SCROLL_NAME)))
        {
            // assert(d_[STRINGIFY(LOCK_MOUSE_ON_SCROLL_NAME)].IsBool());
            TOKENPASTE(LOCK_MOUSE_ON_SCROLL_NAME, _) = d_[STRINGIFY_HELPER(LOCK_MOUSE_ON_SCROLL_NAME)].GetBool();
        }
        else
        {
            printf(STRINGIFY_HELPER(LOCK_MOUSE_ON_SCROLL_NAME) " not found!\n");
        }

        if (d_.HasMember(STRINGIFY_HELPER(SCROLLING_ON_NAME)))
        {
            // assert(d_[STRINGIFY(SCROLLING_ON_NAME)].IsBool());
            TOKENPASTE(SCROLLING_ON_NAME, _) = d_[STRINGIFY_HELPER(SCROLLING_ON_NAME)].GetBool();
        }
        else
        {
            printf(STRINGIFY_HELPER(SCROLLING_ON_NAME) " not found!\n");
        }
        
        if (d_.HasMember(STRINGIFY_HELPER(SCROLLING_SPEED_NAME)))
        {
            // assert(d_[STRINGIFY(SCROLLING_SPEED_NAME)].IsFloat());
            TOKENPASTE(SCROLLING_SPEED_NAME, _) = d_[STRINGIFY_HELPER(SCROLLING_SPEED_NAME)].GetFloat();
        }
        else
        {
            printf(STRINGIFY_HELPER(SCROLLING_SPEED_NAME) " not found!\n");
        }
    }
};
