! { dg-do compile }
!
! Checks that under-specified arrays (referencing arrays with fewer
! dimensions than the array spec) generates a warning.
!
! Contributed by Jim MacArthur <jim.macarthur@codethink.co.uk>
!

program under_specified_array
    INTEGER chsbrd(8,8)
    chsbrd(3,1) = 5
    print *, chsbrd(3) ! { dg-warning "Using the lower bound for unspecified dimensions in array reference" }
end program
