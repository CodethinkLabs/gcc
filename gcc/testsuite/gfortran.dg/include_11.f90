! { dg-do run }
!
! Ensure that we can make an assignment to a variable named
! include.
!

program one
  integer :: include
  include&
 = 555
  if (include.ne.555) stop 1
end program one
