set_project("c-survivors")
set_version("1.0.0")
set_languages("c23")
add_rules("mode.debug", "mode.release")

local libs = { "raylib", "raygui", "flecs", "enet" }

add_includedirs("include")
add_requires(table.unpack(libs))

-- TODO: Add third party build

target("c-survival")
do
	set_kind("binary")
	add_files("include/third_party/**/*.c")
	add_files("src/*.c")
	add_files("src/**/*.c")
	add_packages(table.unpack(libs))
	add_defines("LOG_USE_COLOR")
end

add_installfiles("(include/**)", { prefixdir = "" })

-- 6. Add some frequently-used compilation flags in xmake.lua
--
-- @code
--    -- add debug and release modes
--    add_rules("mode.debug", "mode.release")
--
--    -- add macro definition
--    add_defines("NDEBUG", "_GNU_SOURCE=1")
--
--    -- set warning all as error
--    set_warnings("all", "error")
--
--    -- set language: c99, c++11
--    set_languages("c99", "c++11")
--
--    -- set optimization: none, faster, fastest, smallest
--    set_optimize("fastest")
--
--    -- add include search directories
--    add_includedirs("/usr/include", "/usr/local/include")
--
--    -- add link libraries and search directories
--    add_links("tbox")
--    add_linkdirs("/usr/local/lib", "/usr/lib")
--
--    -- add system link libraries
--    add_syslinks("z", "pthread")
--
--    -- add compilation and link flags
--    add_cxflags("-stdnolib", "-fno-strict-aliasing")
--    add_ldflags("-L/usr/local/lib", "-lpthread", {force = true})
--
-- @endcode
--
