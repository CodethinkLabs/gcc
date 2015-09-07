! { dg-do run }
! { dg-options -funderspecified-arrays }
!
! Checks that under-specified arrays (referencing arrays with fewer
! dimensions than the array spec) generates a warning.
!
! Contributed by Jim MacArthur <jim.macarthur@codethink.co.uk>
!

program under_specified_array
    INTEGER chsbrd(8,8)
    INTEGER a(5:8, 5:8)
    INTEGER c,d
    chsbrd(3,1) = 5
    a(6,5) = 6
    c = chsbrd(3) ! { dg-warning "Using the lower bound for unspecified dimensions in array reference" }
    d = a(6) ! { dg-warning "Using the lower bound for unspecified dimensions in array reference" }
    if (c .ne. 5) call abort
    if (d .ne. 6) call abort
end program
