! { dg-do run }
! { dg-options "-fdec -fno-dec-pad-with-spaces" }
!

program test
  integer(kind=8) :: a
  a = transfer("ABCE", 1_8)
  ! If a has not been converted into big endian
  ! or little endian integer it has failed.
  if ((a.ne.int(z'4142434500000000',kind=8)).and. &
      (a.ne.int(z'45434241',kind=8))) then 
    stop 1
  end if
end program test


