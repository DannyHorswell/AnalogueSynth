# only build I2S example if library is available
if (TARGET pico_audio_i2s)
    add_executable(pico_audio_synth_i2s
        LCRFilter.cpp
        lfo.cpp
        noise.cpp
        pan.cpp
        patch.cpp
        reverb.cpp
        tdl.cpp
        voice.cpp
        synth_main.cpp
        synth.cpp
        wavegenerator.cpp
        signal.cpp
        )

    target_link_libraries(pico_audio_synth_i2s PRIVATE
            pico_stdlib
            pico_audio_i2s
            )

    target_compile_definitions(pico_audio_synth_i2s PRIVATE
            # compile time configuration of I2S
            PICO_AUDIO_I2S_MONO_INPUT=1
            #define for our example code
            USE_AUDIO_I2S=1
            )
    # create map/bin/hex file etc.
    pico_add_extra_outputs(pico_audio_synth_i2s)
endif ()

# only build PWM example if library is available
if (TARGET pico_audio_pwm)
    add_executable(pico_audio_synth_pwm
        LCRFilter.cpp
        lfo.cpp
        noise.cpp
        pan.cpp
        patch.cpp
        reverb.cpp
        tdl.cpp
        voice.cpp
        synth_main.cpp
        synth.cpp
        wavegenerator.cpp
        )

    target_link_libraries(pico_audio_synth_pwm PRIVATE
            pico_stdlib
            pico_audio_pwm
            )

    target_compile_definitions(pico_audio_synth_pwm PRIVATE
            #define for our example code
            USE_AUDIO_PWM=1
            )
    # create map/bin/hex file etc.
    pico_add_extra_outputs(pico_audio_synth_pwm)
endif ()

# only build S/PDIF example if library is available
if (TARGET pico_audio_spdif)
    add_executable(pico_audio_synth_spdif
        LCRFilter.cpp
        lfo.cpp
        noise.cpp
        pan.cpp
        patch.cpp
        reverb.cpp
        tdl.cpp
        voice.cpp
        synth_main.cpp
        synth.cpp
        wavegenerator.cpp
            )

    target_link_libraries(pico_audio_synth_spdif PRIVATE
            pico_stdlib
            pico_audio_spdif
            )

    target_compile_definitions(pico_audio_synth_spdif PRIVATE
            # compile time configuration of S/PDIF
            PICO_AUDIO_SPDIF_MONO_INPUT=1
            #define for our example code
            USE_AUDIO_SPDIF=1
            )
    # create map/bin/hex file etc.
    pico_add_extra_outputs(pico_audio_synth_spdif)
endif ()


# switch stdio to USB from uart
pico_enable_stdio_usb(pico_audio_synth_i2s 1)
pico_enable_stdio_uart(pico_audio_synth_i2s 0)
