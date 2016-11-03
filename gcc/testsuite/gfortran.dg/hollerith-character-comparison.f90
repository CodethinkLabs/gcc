! { dg-do run }
! { dg-options "-fdec -Wconversion" }

      program convert
      CHARACTER*4 a
      a = "TEST"

      if (4HTEST.ne.4HTEST) call abort() ! { dg-warning "Conversion from HOLLERITH to CHARACTER\\(1\\)" }
      if (4HTEST.ne."TEST") call abort() ! { dg-warning "Conversion from HOLLERITH to CHARACTER\\(1\\)" }
      if (4HTEST.eq."ZEST") call abort() ! { dg-warning "Conversion from HOLLERITH to CHARACTER\\(1\\)" }
      if ("TEST".eq.4HZEST) call abort() !  { dg-warning "Conversion from HOLLERITH to CHARACTER\\(1\\)" }
      if ("AAAA".eq. 5HAAAAA) call abort() ! { dg-warning "Conversion from HOLLERITH to CHARACTER\\(1\\)" }
      if ("BBBBB".eq. 4HBBBB) call abort() ! { dg-warning "Conversion from HOLLERITH to CHARACTER\\(1\\)" }
      if (4HTEST.ne.a) call abort() !  { dg-warning "Conversion from HOLLERITH to CHARACTER\\(1\\)" }
      end program

