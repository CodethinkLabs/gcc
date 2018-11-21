! { dg-do run }
! { dg-options "-fdec -fpad-source" }
!
! Contributed by Mark Eggleston <mark.eggleston@codethink.com>
!
      program test
      implicit none
      character(len=40) :: a_long_string = "abcdef                        
     +gh"
      if (a_long_string.ne."abcdef                      gh") stop 1
      end program test
