
x86 obfuscator - source code


Files:

bin\
    loader\loader.exe             - compiled loader
    protect.exe                   - compiled virtualizer
    test_app\vm_test.exe          - compiled sample application
    test_app\vm_test_vmed_01.exe  - sample app with one vm layer
    test_app\vm_test_vmed_02.exe  - sample app with two vm layers

doc\
    x86.virt.after.gif        - diagram of executable after virtualization
    x86.virt.before.gif       - diagram executable before virtualization
    x86.virt.pdf              - documentation


loader\
    loader.asm                - loader source code

protector\
    macro.h                   - auxiliary macros
    common.cpp                - binary version of poly_(enc/dec) function
    common.h                  - header for above
    hde.h                     - header for Hacker Disassembler Engine
    hde.lib                   - Hacker Disassembler Engine library
    main.cpp                  - main program (gui)
    protect.cpp               - core virtualization engine, PE handling
    protect.h                 - header for above
    res.rc                    - resources
    resource.h                - header for above


test_app\
    main.cpp                  - sample application
    test_app\res.rc           - resources
    test_app\resource.h       - header for above

