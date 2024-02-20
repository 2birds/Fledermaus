#pragma once

#include "rapidjson/document.h"
#include <fstream>
#include <sstream>
#include <string>

#define CONFIG_FILE_NAME "fledermaus_config.json"

#define SPEED_NAME "Speed"
#define SCROLLING_ON_NAME "ScrollingActive"
#define SCROLLING_SPEED_NAME "ScrollingSpeed"
#define ORIENTATION_NAME "Orientation"

namespace rjs=rapidjson;


class ConfigReader {
    private:
    std::string config_file_name_;
    rjs::Document d_;

    float speed_ = 2.0f;
    bool use_scrolling_ = true;
    float scrolling_speed_ = 2.0f;
    bool vertical_orientation_ = true;

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
    }

    ConfigReader() : ConfigReader(CONFIG_FILE_NAME)
    {}

    ~ConfigReader() {}

    void SetSpeed(const float speed)
    {
        speed_ = speed;
    }

    void SetUseScrolling(const bool use_scrolling)
    {
        use_scrolling_ = use_scrolling;
    }

    void SetScrollingSpeed(const float scrollingSpeed)
    {
        scrolling_speed_ = scrollingSpeed;
    }

    void SetVerticalOrientation(const bool v)
    {
        vertical_orientation_ = v;
    }

    void SetHorizontalOrientation(const bool h)
    {
        vertical_orientation_ = !h;
    }

    float GetSpeed() const
    {
        return speed_;
    }

    bool GetUseScrolling() const
    {
        return use_scrolling_;
    }

    float GetScrollingSpeed() const
    {
        return scrolling_speed_;
    }

    bool IsVerticalOrientation() const
    {
        return vertical_orientation_;
    }

    bool IsHorizontalOrientation() const
    {
        return !vertical_orientation_;
    }

    private:
    void validateAndLoadJson()
    {
        if (d_.HasMember(ORIENTATION_NAME))
        {
            assert(d_[ORIENTATION_NAME].IsString());

            if (d_[ORIENTATION_NAME].GetString() == "vertical")
            {
                vertical_orientation_ = true;
            }
            else if (d_[ORIENTATION_NAME].GetString() == "horizontal")
            {
                vertical_orientation_ = false;
            }
            else
            {
                // Use default. This is an error condition really
            }
        }
        
        if (d_.HasMember(SPEED_NAME))
        {
            assert(d_[SPEED_NAME].IsNumber());
            speed_ = d_[SPEED_NAME].GetFloat();
        }

        if (d_.HasMember(SCROLLING_ON_NAME))
        {
            assert(d_[SCROLLING_ON_NAME].IsBool());
            use_scrolling_= d_[SCROLLING_ON_NAME].GetBool();
        }
        
        if (d_.HasMember(SCROLLING_SPEED_NAME))
        {
            assert(d_[SCROLLING_SPEED_NAME].IsFloat());
            speed_ = d_[SCROLLING_SPEED_NAME].GetFloat();
        }
    }
};