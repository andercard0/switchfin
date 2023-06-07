package("mpv")
    if is_plat("windows", "mingw") then
        set_urls("https://github.com/shinchiro/mpv-winbuild-cmake/releases/download/20230531/mpv-dev-x86_64-20230531-git-f794584.7z")
        add_versions("20230531", "d102e531af71fdeb55f1dfa6a2f177a745347900e3e7bb553f07862d1dbeecc5")
    end
    add_links("mpv")
    on_install("windows", "mingw", function (package)
        os.cp("include/*", package:installdir("include").."/")
        os.cp("*.a", package:installdir("lib").."/")
        os.cp("*.dll", package:installdir("bin").."/")

        -- 从 dll 里导出函数为 lib 文件，预编译自带 def 文件格式不正确，没法导出 lib
        if os.isfile("mpv.def") then
            local def_context = io.readfile("mpv.def")
            if not def_context:startswith("EXPORTS") then
                io.writefile("mpv.def", format("EXPORTS\n%s", def_context))
            end
        end
        
        local find_vstudio = import("detect.sdks.find_vstudio")
        for _, vsinfo in pairs(find_vstudio()) do
            if vsinfo.vcvarsall then
                os.setenv("PATH", vsinfo.vcvarsall["x64"]["PATH"])
            end
        end

        os.execv("lib.exe", {"/name:libmpv-2.dll", "/def:mpv.def", "/out:mpv.lib", "/MACHINE:X64"})
        os.cp("*.lib", package:installdir("lib").."/")
        os.cp("*.exp", package:installdir("lib").."/")
    end)