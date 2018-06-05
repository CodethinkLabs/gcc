! { dg-do compile }
!
! Ensure we can make an assignment to a variable named include using
! a line continuation
!

subroutine one()
  integer :: include
  include &
     = 5
end subroutine one
