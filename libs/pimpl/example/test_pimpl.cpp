// Copyright (c) 2009 Vladimir Batov.
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. See http://www.boost.org/LICENSE_1_0.txt.

#include "test_pimpl.hpp"
#include <string>
#include <fstream>

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

struct MooBase
{
    typedef boost::safebool<MooBase> safebool;

    explicit MooBase(int v) : value_(v) {}
    operator safebool::type() const { return safebool(value_); }

    bool operator==(MooBase const& that) { return value_ == that.value_; }
    bool operator!=(MooBase const& that) { return value_ != that.value_; }

    int value_;
};

struct Moo : public MooBase
{
    Moo(int v) : MooBase(v) {}

    int more_value_;
};


int
main(int argc, char const* argv[])
{
    //	printf("_MSC_VER=%d\n", _MSC_VER);

	singleton_type singleton;
    pass_value_type pass_value;

    Moo              moo(5);
    Foo              foo;
    Foo const  const_foo = foo;
    Foo&             ref = foo;
    Foo const& const_ref = const_foo;
    Foo*             ptr = &foo;
    Foo const* const_ptr = &const_foo;

    BOOST_ASSERT(false == is_pimpl<Foo>::value);
    BOOST_ASSERT(false == is_pimpl<int>::value);
    BOOST_ASSERT(false == is_pimpl<int*>::value);
    BOOST_ASSERT(false == is_pimpl<int const&>::value);
    BOOST_ASSERT(true  == is_pimpl<Test1>::value);
    BOOST_ASSERT(false == is_pimpl<Test1*>::value);
    BOOST_ASSERT(true  == is_pimpl<Test2>::value);
    BOOST_ASSERT(true  == is_pimpl<Base>::value);
    BOOST_ASSERT(true  == is_pimpl<Derived1>::value);
    BOOST_ASSERT(true  == is_pimpl<Derived1 const>::value);
    BOOST_ASSERT(true  == is_pimpl<Derived1 const&>::value);

    Test1 pt11;                         BOOST_ASSERT(pt11.trace() == "Test1::implementation()");
    Test2 vt11;                         BOOST_ASSERT(vt11.trace() == "Test2::implementation()");
    Test1 pt12(5);                      BOOST_ASSERT(pt12.trace() == "Test1::implementation(int)");
    Test2 vt12(5);                      BOOST_ASSERT(vt12.trace() == "Test2::implementation(int)");
    Test1 pt13 = pt12;                  BOOST_ASSERT(pt13.id() == pt12.id()); // No copying. Implementation shared.
    Test2 vt13 = vt12;                  BOOST_ASSERT(vt13.trace() == "Test2::implementation(implementation const&)");
                                        BOOST_ASSERT(vt13.id() != vt12.id()); // Implementation copied.
    Test1 pt14(pt12);                   BOOST_ASSERT(pt14.id() == pt12.id()); // No copying. Implementation shared.
    Test2 vt14(vt12);                   BOOST_ASSERT(vt14.trace() == "Test2::implementation(implementation const&)");
                                        BOOST_ASSERT(vt14.id() != vt12.id()); // Implementation copied.
    Test1 pt15(5, 6);                   BOOST_ASSERT(pt15.trace() == "Test1::implementation(int, int)");
    Test1 pt16(singleton);
    Test1 pt17(singleton);              BOOST_ASSERT(pt16.id() == pt17.id()); // No copying. Implementation shared.

    Test1 pt21(foo);                    //BOOST_ASSERT(pt21.trace() == "Test1::implementation(Foo&)");
    Test1 pt22(const_foo);              BOOST_ASSERT(pt22.trace() == "Test1::implementation(Foo const&)");
    Test1 pt23(ref);                    BOOST_ASSERT(pt23.trace() == "Test1::implementation(Foo&)");
    Test1 pt24(const_ref);              BOOST_ASSERT(pt24.trace() == "Test1::implementation(Foo const&)");
    Test1 pt25(ptr);                    BOOST_ASSERT(pt25.trace() == "Test1::implementation(Foo*)");
    Test1 pt26(const_ptr);              BOOST_ASSERT(pt26.trace() == "Test1::implementation(Foo const*)");

    Test1 pt31(const_foo, const_foo);   BOOST_ASSERT(pt31.trace() == "Test1::implementation(Foo const&, Foo const&)");
    Test1 pt32(foo, const_foo);         BOOST_ASSERT(pt32.trace() == "Test1::implementation(Foo&, Foo const&)");
    Test1 pt33(const_foo, foo);         BOOST_ASSERT(pt33.trace() == "Test1::implementation(Foo const&, Foo&)");
    Test1 pt34(foo, foo);               BOOST_ASSERT(pt34.trace() == "Test1::implementation(Foo&, Foo&)");
    Test1 pt35(foo, Foo());             BOOST_ASSERT(pt35.trace() == "Test1::implementation(Foo&, Foo const&)");
    Test1 pt36(Foo(), foo);             BOOST_ASSERT(pt36.trace() == "Test1::implementation(Foo const&, Foo&)");
    Test1 pt37(pass_value);             BOOST_ASSERT(pt37.trace() == "Test1::implementation(Foo const&)");

    ////////////////////////////////////////////////////////////////////////////
    // Comparisons with 'int'. g++-4.3.4 (linux).
    ////////////////////////////////////////////////////////////////////////////
    // The standard behavior.
//  bool moo_check1 = moo == int(5); // error: no match for ‘operator==’ in ‘moo == 5’
//  bool moo_check2 = moo != int(5); // error: no match for ‘operator!=’ in ‘moo != 5’
//  bool moo_check3 = int(5) == moo; // error: no match for ‘operator==’ in ‘5 == moo’
//  bool moo_check4 = int(5) != moo; // error: no match for ‘operator!=’ in ‘5 != moo’
    // The pointer Pimpl behavior.
//  bool   pcheck01 = pt12 == int(5); // error: no match for ‘operator==’ in ‘pt12 == 5’
//  bool   pcheck02 = pt12 != int(5); // error: no match for ‘operator!=’ in ‘pt12 != 5’
//  bool   pcheck03 = int(5) == pt12; // error: no match for ‘operator==’ in ‘5 == pt12’
//  bool   pcheck04 = int(5) != pt12; // error: no match for ‘operator!=’ in ‘5 != pt12’
    // The value Pimpl behavior.
    bool   vcheck01 = vt12 == int(5); // Test2 has its own op==(). Non-explicit Test2(int) called.
    bool   vcheck02 = vt12 != int(5); // Test2 has its own op!=(). Non-explicit Test2(int) called.
//  bool   vcheck03 = int(5) == vt12; // error: no match for ‘operator==’ in ‘5 == vt12’
//  bool   vcheck04 = int(5) != vt12; // error: no match for ‘operator!=’ in ‘5 != vt12’

    ////////////////////////////////////////////////////////////////////////////
    // Comparisons with 'bool'. g++-4.3.4 (linux).
    ////////////////////////////////////////////////////////////////////////////
    // The standard behavior.
    bool moo_check5 = moo == false; // Deploys operator safebool::type() conversion
    bool moo_check6 = moo != false; // Deploys operator safebool::type() conversion
    bool moo_check7 = false == moo; // Deploys operator safebool::type() conversion
    bool moo_check8 = false != moo; // Deploys operator safebool::type() conversion
    // The pointer Pimpl behavior.
    bool pcheck05 = pt12 == false;   // Deploys operator safebool::type() conversion
    bool pcheck06 = pt12 != false;   // Deploys operator safebool::type() conversion
    bool pcheck07 = false == pt12;   // Deploys operator safebool::type() conversion
    bool pcheck08 = false != pt12;   // Deploys operator safebool::type() conversion
    // The value Pimpl behavior.
//  bool vcheck05 = vt12 == false;   // Test2 has its own op==(). error: ambiguous overload for ‘operator==’ in ‘vt12 == false’
//  bool vcheck06 = vt12 != false;   // Test2 has its own op!=(). error: ambiguous overload for ‘operator!=’ in ‘vt12 != false’
    bool vcheck07 = false == vt12;   // Deploys operator safebool::type() conversion
    bool vcheck08 = false != vt12;   // Deploys operator safebool::type() conversion

    ////////////////////////////////////////////////////////////////////////////
    // Comparisons with Pimpl. g++-4.3.4 (linux).
    ////////////////////////////////////////////////////////////////////////////
    bool pcheck1 = pt12 == pt13;    // calls pimpl_base::op==()
    bool pcheck2 = pt12 != pt13;    // calls pimpl_base::op!=()
    bool vcheck1 = vt12 == vt13;    // calls Test2::op==()
    bool vcheck2 = vt12 != vt13;    // calls Test2::op!=()

    if (  moo) {} // Check it calls conversion to safebool.
    if ( !moo) {} // Check it calls conversion to safebool.
    if ( pt11) {} // Check it calls conversion to safebool.
    if (!pt11) {} // Check it calls conversion to safebool.
    if ( vt11) {} // Check it calls conversion to safebool.
    if (!vt11) {} // Check it calls conversion to safebool.

    {   // Testing swap().
        int pt16_id = pt16.id();
        int pt17_id = pt17.id();
        int vt13_id = vt13.id();
        int vt14_id = vt14.id();

        pt16.swap(pt17);
        vt13.swap(vt14);

        BOOST_ASSERT(pt16.id() == pt17_id);
        BOOST_ASSERT(pt17.id() == pt16_id);
        BOOST_ASSERT(vt13.id() == vt14_id);
        BOOST_ASSERT(vt14.id() == vt13_id);
    }
    int k11 = vt11.get(); BOOST_ASSERT(k11 ==  0);
    int k12 = vt12.get(); BOOST_ASSERT(k12 ==  5);
    int k13 = vt13.get(); BOOST_ASSERT(k13 ==  5);

    vt14.set(33);

    BOOST_ASSERT(vt12 == vt13); // Good: Only compiles if op==() is part of Test2 interface.
    BOOST_ASSERT(vt12.trace() == "Test2::operator==(Test2 const&)");
    BOOST_ASSERT(vt11 != vt12); // Good: Only compiles if op==() is part of Test2 interface.
    BOOST_ASSERT(vt11.trace() == "Test2::operator==(Test2 const&)");

    {   // Testing run-time polymorphic behavior.
        Base        base1 (1);
        Derived1 derived1 (2, 3);
        Derived2 derived2 (2, 3, 4);
        Base        base2 (derived1); // calls copy constructor
        Base        base3 (derived2); // calls copy constructor
        Base        base4 (Derived2(2,3,4)/*const ref to temporary*/); // calls copy constructor
        Derived1     bad1 (pimpl<Derived1>::null());
        Derived2     bad2 (pimpl<Derived2>::null());
        Base*         bp1 = &base1;
        Base*         bp2 = &base2;
        Base*         bp3 = &base3;
        Base*         bp4 = &derived1;
        Base*         bp5 = &derived2;
        Base*         bp6 = &bad1;
        Base*         bp7 = &bad2;
        Base        bad_base1 = Base::null();
        Base        bad_base2 = pimpl<Base>::null();
//      Base        bad_base3 = pimpl<Foo>::null(); // correctly does not compile.
//      Foo           bad_foo = pimpl<Foo>::null(); // correctly does not compile.
        Derived1 bad_derived1 = pimpl<Derived1>::null();
        Derived2 bad_derived2 = pimpl<Derived2>::null();

        bool check1 = base1 == derived1;
        bool check2 = base1 == derived2;
        bool check3 = derived1 == base1;
        bool check4 = derived2 == base1;

        if ( bad1) BOOST_ASSERT(0);
        if (!bad2) BOOST_ASSERT(1);

        base2.call_virtual(); BOOST_ASSERT(base2.trace() == "Derived1::call_virtual()");
        base3.call_virtual(); BOOST_ASSERT(base3.trace() == "Derived2::call_virtual()");

        bp1->call_virtual();  BOOST_ASSERT(bp1->trace() == "Base::call_virtual()");
        bp2->call_virtual();  BOOST_ASSERT(bp2->trace() == "Derived1::call_virtual()");
        bp3->call_virtual();  BOOST_ASSERT(bp3->trace() == "Derived2::call_virtual()");
        bp4->call_virtual();  BOOST_ASSERT(bp4->trace() == "Derived1::call_virtual()");
        bp5->call_virtual();  BOOST_ASSERT(bp5->trace() == "Derived2::call_virtual()");
//      bp6->call_virtual();  // crash. referencing bad1
//      bp7->call_virtual();  // crash. referencing bad1
    }
#ifdef __linux__
    {
        printf("BEG: Testing pimpl and boost::serialization.\n");
        Test1                      saved (12345);
        std::ofstream                ofs ("filename");
        boost::archive::text_oarchive oa (ofs);

        printf("Calling boost::archive::test_oarchive << Test1.\n");
        oa << *(Test1 const*) &saved; // write class instance to archive
        ofs.close();

        std::ifstream                ifs ("filename", std::ios::binary);
        boost::archive::text_iarchive ia (ifs);
        Test1                   restored (Test1::null()); // Implementation will be replaced.

        printf("Calling boost::archive::test_iarchive >> Test1.\n");
        ia >> restored; // read class state from archive
        assert(saved.get() == restored.get());
        printf("END: Testing pimpl and boost::serialization.\n");
    }
#endif
    printf("Pimpl test has successfully completed.\n");
    return 0;
}

