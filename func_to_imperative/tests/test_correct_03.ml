let f : int -> int -> int -> int = fun a_1 a_2 a_3 ->
	a_1 + a_2 * a_3
in f 4;;

let f : int -> int -> int -> int = fun a ->
	let g : int -> int -> int = fun b c ->
		a + b + c
	in g
;;

let f : int -> int -> int -> int = fun a ->
	if (a = 0) then
		let g : int -> int -> int = fun a b ->
			a + b
		in g  (* int(int, int) *)
	else
		let g : int -> int -> int = fun a ->
			let m : int -> int = fun b ->
				a * a - b
			in m
		in g (* function<int (int)>(int)  *)
;;
