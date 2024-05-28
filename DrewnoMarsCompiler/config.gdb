set breakpoint pending on
set confirm off
file ./dmc
break drewno_mars::Err::report
commands
	where
end
break drewno_mars::ToDoError::ToDoError
commands
	where
end
break drewno_mars::InternalError::InternalError
commands
	where
end

define t7
  set args p7_tests/$arg0.dm -o --
  run
end
