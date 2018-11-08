! { dg-do run }
!
! Ensure we can make an assignment to a variable named include using
! a line continuation and insterspersed comment
!

program test
  integer :: include
  include &
!comment
      = 15
  if (include.ne.15) stop 1
end program test
