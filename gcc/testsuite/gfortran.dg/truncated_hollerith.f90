! { dg-do compile }
! { dg-options "-Wcharacter-truncation -std=legacy" }
!
! Test case contributed by Mark Eggleston  <mark.eggleston@codethink.com>

program test
  character(4) :: a = 4hABCD
  character(4) :: b = 5hABCDE ! { dg-warning "is being truncated \\(4/5\\)" }

end program 
