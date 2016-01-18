! { dg-do compile }
! { dg-options "-std=gnu -ffixed-form" }

      include '././././././././././././././././././././././include_long.inc'
      program main
      end program

      ! { dg-warning "longer than" " " { target *-*-* } 4 }

