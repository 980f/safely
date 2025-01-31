get_target_property(arf ${safely_target} "INCLUDE_DIRECTORIES")

message("Explicit includes:")
foreach (def_elem ${arf})
  message ("${def_elem}")
endforeach()

get_target_property(arf ${safely_target} "COMPILE_DEFINITIONS")

message("Explicit defines:")
foreach (def_elem ${arf})
  message ("${def_elem}")
endforeach()
#someday test for known names and annotate them, until then:
#message("IoSource performance tweak: ${SafelyIoSourceEvents}")
#message("Epoller performance tweak: ${SafelyApplicationEvents}")
#message("Whether dynamic memory allocation is allowed: ${SafelyHasHeap}")
