//
// Created by fang on 2022/8/12.
//

#pragma once

#include <borealis.hpp>
#include <borealis/core/singleton.hpp>
#include <mpv/client.h>
#ifdef MPV_SW_RENDER
#include <mpv/render.h>
#elif defined(BOREALIS_USE_D3D11)
#include <mpv/render_dxgi.h>
#elif defined(BOREALIS_USE_DEKO3D)
#include <mpv/render_dk3d.h>
#else
#include <mpv/render_gl.h>
#endif

typedef enum MpvEventEnum {
    MPV_LOADED,
    MPV_PAUSE,
    MPV_RESUME,
    MPV_STOP,
    LOADING_START,
    LOADING_END,
    UPDATE_DURATION,
    UPDATE_PROGRESS,
    START_FILE,
    END_OF_FILE,
    CACHE_SPEED_CHANGE,
    VIDEO_SPEED_CHANGE,
    MPV_FILE_ERROR,
} MpvEventEnum;

typedef brls::Event<MpvEventEnum> MPVEvent;
typedef std::unordered_map<std::string, std::string> MPVMap;

class MPVCore : public brls::Singleton<MPVCore> {
public:
    MPVCore();

    ~MPVCore() = default;

    void restart();

    void init();

    void clean();

    void command_str(const char *args);

    template <typename... T>
    void command_str(std::string_view fmt, T &&...args) {
        std::string cmd = fmt::format(fmt::runtime(fmt), std::forward<T>(args)...);
        command_str(cmd.c_str());
    }

    void command_async(const char **args, uint64_t reply_userdata = 0);

    bool isStopped();

    bool isPaused();

    double getSpeed();

    std::string getCacheSpeed() const;

    void setUrl(const std::string &url, const std::string &extra = "", const std::string &method = "replace");

    std::string getString(const std::string &key);

    double getDouble(const std::string &key);
    void setDouble(const std::string &key, double value);

    int64_t getInt(const std::string &key, int64_t default_value = 0);
    void setInt(const std::string &key, int64_t value);

    std::unordered_map<std::string, mpv_node> getNodeMap(const std::string &key);

    void togglePlay();

    void stop();

    void seek(int64_t p);

    void setSpeed(double value);

    void setFrameSize(brls::Rect rect);

    bool isValid();

    static void disableDimming(bool disable);

    void draw(brls::Rect rect, float alpha = 1.0);

    /**
     * 播放器内部事件
     * 传递内容为: 事件类型
     */
    MPVEvent &getEvent() { return this->mpvCoreEvent; }

    void reset();

    void setShader(const std::string &profile, const std::string &shaders, bool showHint = true);

    void clearShader(bool showHint = true);

    MPVMap supportCodecs();

    // core states
    int64_t duration = 0;  // second
    int64_t video_progress = 0;
    int64_t cache_speed = 0;  // Bps
    double video_speed = 0;
    double playback_time = 0;
    double percent_pos = 0;

    inline static bool DEBUG = false;

    // Bottom progress bar
    inline static bool BOTTOM_BAR = true;
    inline static bool OSD_ON_TOGGLE = true;

    // 低画质解码，剔除解码过程中的部分步骤，可以用来节省cpu
    inline static bool LOW_QUALITY = false;

    // 视频缓存（是否使用内存缓存视频，值为缓存的大小，单位MB）
    inline static int INMEMORY_CACHE = 0;

    // 硬件解码
    inline static bool HARDWARE_DEC = false;
    inline static std::string PLAYER_HWDEC_METHOD = "auto";
    inline static std::string VIDEO_CODEC = "h264";
    inline static std::vector<int64_t> MAX_BITRATE = {0, 10000000, 8000000, 4000000, 2000000};

    inline static bool FORCE_DIRECTPLAY = false;

    // 触发倍速时的默认值，单位为 %
    inline static int VIDEO_SPEED = 200;
    inline static int SEEKING_STEP = 15;

    NVGcolor bottomBarColor = brls::Application::getTheme().getColor("color/app");

private:
    mpv_handle *mpv = nullptr;
    mpv_render_context *mpv_context = nullptr;
#ifdef MPV_SW_RENDER
    const int PIXCEL_SIZE = 4;
    int nvg_image = 0;
    const char *sw_format = "rgba";
    int sw_size[2] = {
        (int)brls::Application::windowWidth,
        (int)brls::Application::windowHeight,
    };
    size_t pitch = PIXCEL_SIZE * sw_size[0];
    void *pixels = nullptr;
    mpv_render_param mpv_params[5] = {
        {MPV_RENDER_PARAM_SW_SIZE, &sw_size[0]},
        {MPV_RENDER_PARAM_SW_FORMAT, (void *)sw_format},
        {MPV_RENDER_PARAM_SW_STRIDE, &pitch},
        {MPV_RENDER_PARAM_SW_POINTER, pixels},
        {MPV_RENDER_PARAM_INVALID, nullptr},
    };
#elif defined(BOREALIS_USE_D3D11)
    mpv_render_param mpv_params[1] = {
        {MPV_RENDER_PARAM_INVALID, nullptr},
    };
#elif defined(BOREALIS_USE_DEKO3D)
    DkFence doneFence;
    DkFence readyFence;
    mpv_deko3d_fbo mpv_fbo{nullptr, &readyFence, &doneFence, 1280, 720, DkImageFormat_RGBA8_Unorm};
    mpv_render_param mpv_params[3] = {
        {MPV_RENDER_PARAM_DEKO3D_FBO, &mpv_fbo},
        {MPV_RENDER_PARAM_INVALID, nullptr},
    };
#else
    mpv_opengl_fbo mpv_fbo;
    int flip_y{1};
    mpv_render_param mpv_params[3] = {
        {MPV_RENDER_PARAM_OPENGL_FBO, &mpv_fbo},
        {MPV_RENDER_PARAM_FLIP_Y, &flip_y},
        {MPV_RENDER_PARAM_INVALID, nullptr},
    };
#endif

    // MPV 内部事件，传递内容为: 事件类型
    MPVEvent mpvCoreEvent;

    // 当前软件是否在前台的回调
    brls::Event<bool>::Subscription focusSubscription;

    /// Will be called in main thread to get events from mpv core
    void eventMainLoop();

    static void on_update(void *self);
    static void on_wakeup(void *self);
};
