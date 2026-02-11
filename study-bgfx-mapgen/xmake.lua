


set_policy("build.progress_style", "multirow")
add_rules("mode.debug", "mode.release")
add_requires("bgfx", {configs = {debug = true, shared = false, shader_include = true}})
add_requires("glfw", {configs = {shared = false}})
set_defaultmode("debug")
set_runtimes("MTd")
--add_rules("@bgfx/shaders")

rule("bgfx_shader")
    set_extensions(".vert", ".frag")  -- 
    on_build_file(function (target, sourcefile, opt)
        --local shaderc = "D:/git/bgfx.cmake/build/cmake/bgfx/Debug/shaderc.exe"
        local ext = path.extension(sourcefile):lower()
        local shaderc = "shadercDebug"  -- 
        local shader_dir = path.join(os.scriptdir(), "shaders")
        local gen_dir = target:targetdir()
        local varying = path.join(shader_dir, "varying.def.sc")

        -- 
        local basename = path.basename(sourcefile)
        
        local shader_type
        if ext == ".vert" then
            shader_type = "vertex"
        elseif ext == ".frag" then
            shader_type = "fragment"
        else
            raise("unsupported shader type: %s", ext)
        end

        -- 
        local render_types = {
            { name = "glsl", profile = "130" },
            { name = "essl", profile = "320_es" }
        }
        
        local outputs = {}

        for _, rt in ipairs(render_types) do
            local out_dir = path.join(gen_dir, "shaders", rt.name)
            os.mkdir(out_dir)

            local output_file = path.join(out_dir, basename .. ext .. ".sc.bin")

            -- 
            local argv = {                
                "-f", sourcefile,
                "-o", output_file,
                "--type", shader_type,
                "--profile", rt.profile,
                "--platform", "windows",
                "-i", shader_dir,
                "--varyingdef", varying
            }

            print("Compiling %s → %s (%s)", basename, output_file, rt.name)
            --os.execv(table.unpack(cmd))
            os.execv(shaderc, argv)

            --target:add("files", output_file)
            table.insert(outputs, output_file)
        end
        return outputs
    end)

target("game")
    set_kind("binary")
    add_files("src/*.cpp")    
    add_files("shaders/s**.vert", {rule = "bgfx_shader"})
    add_files("shaders/s**.frag", {rule = "bgfx_shader"})
    add_packages("bgfx", "glfw", "bx")    
    add_includedirs("include")
    set_languages("c++17")
    