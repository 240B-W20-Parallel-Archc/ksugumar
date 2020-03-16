echo -e "\n\n==============="
echo -e "Memory Hierarchy"
echo -e "================\n"

echo -e "L1 Specifications"
echo -e "-----------------"

L1_size=$(</sys/devices/system/cpu/cpu0/cache/index0/size)
L1_linesize=$(</sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size)
L1_assoc=$(</sys/devices/system/cpu/cpu0/cache/index0/ways_of_associativity)
L1_sets=$(</sys/devices/system/cpu/cpu0/cache/index0/number_of_sets)
L1_shared=$(</sys/devices/system/cpu/cpu0/cache/index0/shared_cpu_list)
L1_type=$(</sys/devices/system/cpu/cpu0/cache/index0/type)
L1_level=$(</sys/devices/system/cpu/cpu0/cache/index0/level)

printf "
level: $L1_level
type: $L1_type
size: $L1_size
line_size: $L1_linesize
associativity: $L1_assoc
sets: $L1_sets
shared_cpu_list: $L1_shared
"


echo -e "\n\nL2 Specifications"
echo -e "-----------------"

L2_level=$(</sys/devices/system/cpu/cpu0/cache/index2/level)
L2_type=$(</sys/devices/system/cpu/cpu0/cache/index2/type)
L2_size=$(</sys/devices/system/cpu/cpu0/cache/index2/size)
L2_linesize=$(</sys/devices/system/cpu/cpu0/cache/index2/coherency_line_size)
L2_assoc=$(</sys/devices/system/cpu/cpu0/cache/index2/ways_of_associativity)
L2_sets=$(</sys/devices/system/cpu/cpu0/cache/index2/number_of_sets)
L2_shared=$(</sys/devices/system/cpu/cpu0/cache/index2/shared_cpu_list)

printf "
level: $L2_level
type: $L2_type
size: $L2_size
line_size: $L2_linesize
associativity: $L2_assoc
sets: $L2_sets
shared_cpu_list: $L2_shared
"


echo -e "\n\nL3 Specifications"
echo -e "-----------------"

L3_level=$(</sys/devices/system/cpu/cpu0/cache/index3/level)
L3_type=$(</sys/devices/system/cpu/cpu0/cache/index3/type)
L3_size=$(</sys/devices/system/cpu/cpu0/cache/index3/size)
L3_linesize=$(</sys/devices/system/cpu/cpu0/cache/index3/coherency_line_size)
L3_assoc=$(</sys/devices/system/cpu/cpu0/cache/index3/ways_of_associativity)
L3_sets=$(</sys/devices/system/cpu/cpu0/cache/index3/number_of_sets)
L3_shared=$(</sys/devices/system/cpu/cpu0/cache/index3/shared_cpu_list)

printf "
level: $L3_level
type: $L3_type
size: $L3_size
line_size: $L3_linesize
associativity: $L3_assoc
sets: $L3_sets
shared_cpu_list: $L3_shared
"

echo -e "\n\nMemory Specifications"
echo -e "-----------------"

memsize="$(cat /proc/meminfo | grep MemTotal | grep -Eo '[0-9]{1,10}')"

printf "
Main Memory Size: $memsize kB
"
echo -e "\n"
