(* sample *)
let rec fact : int -> int = fun a ->
	if (a <= 1) then
		1
	else
		a * (fact (a - 1))
;;
