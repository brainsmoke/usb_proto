
n_keys=$(("$1"))

if [ "$n_keys" -lt 1 ]; then
	echo "bad number of keys" >&2

	echo "Usage: $0 <number-of-keys>"
	echo 
	echo "Creates: keypad_<number-of-keys>_keys_{top,bottom}.stl"
	exit 1
fi

echo generating keypad_"$n_keys"_keys_top.stl >&2
openscad -o keypad_"$n_keys"_keys_top.stl -D n_keys="$n_keys" keypad_flush_top_mcu_tiny.scad &
echo generating keypad_"$n_keys"_keys_bottom.stl >&2
openscad -o keypad_"$n_keys"_keys_bottom.stl -D n_keys="$n_keys" keypad_flush_bottom_mcu_tiny.scad &

wait
wait
