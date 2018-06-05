! { dg-do compile }
!
! Ensure that we can make an assignment to a variable named
! include.
!

subroutine one()
  integer :: include
  include = 5
end subroutine one
