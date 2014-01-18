#include <cassert>
#include <iomanip>
#include <iostream>

#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>

#include <qfcl/data_structures/skip_list.hpp>

template<typename SkipList>
void print_structure(std::ostream& os, SkipList const& sl, std::string const& name)
{
    std::cout << name << " structure:" << std::endl << internal_structure(sl) << std::endl;
}

template<typename SkipList, typename Range>
void create_skip_list(SkipList& sl, Range const& range)
{
    using namespace boost;

    for_each(range, [&] (typename SkipList::value_type x)
        {
            sl.insert(x);
        }
    );
}

template<typename SkipList, typename Range>
void build_and_destroy_skip_list(SkipList& sl, std::string const& name, Range const& range)
{
    using namespace boost;

    print_structure(std::cout, sl, name);

    for_each(range, [&] (typename SkipList::value_type x)
        {
            sl.insert(x);
            print_structure(std::cout, sl, name);
        }
    );

    for_each(range, [&] (typename SkipList::value_type x)
        {
            sl.erase(x);
            print_structure(std::cout, sl, name);
        }
    );
}

int main()
{
    using namespace std;
//    using namespace boost;
    using namespace qfcl::container;

    typedef augmented_skip_list::multiset<char> list_type;

//    set<char> s1, s2;
//    indexed_set<char> is1;
//    multiset<char> ms1, ms2;
//    indexed_multiset<char> ims1;

    string const list = "FOURSQUARE";
    const char * alphabet = "ZYXWVUTSRQPONMLKJIHGFEDCBA";

//    set<char> s1;
//    string const s1_name = "set";
//    build_and_destroy_skip_list(s1, s1_name, list);

//    indexed_set<char> isl;
//    string const isl_name = "indexed set";
//    build_and_destroy_skip_list(isl, isl_name, list);

    // test only
    //BOOST_MPL_ASSERT((boost::mpl::not_<skip_list::iterator::is_impl<typename list_type::iterator> >));
    //typename list_type::test_type test;
    //typedef typename list_type::bound_iterator_impl test;
  //  typedef typename boost::mpl::apply<test,boost::mpl::false_>::type base;
    typename list_type::iterator iter;
  //  skip_list::iterator::skip_list_iterator<boost::mpl::identity<skip_list::iterator::skip_list_iterator_base_imp<augmented_node_type<char, std::less<char> >, boost::mpl::_> > > it;
  //  auto it2(it);
  //  ++it;
  //  std::advance(it, 1);
    typename list_type::iterator iter_(iter);
    typename list_type::const_iterator citer(iter);
    assert(iter == citer);

    list_type ms;
    string const ms_name = "multiset";

    create_skip_list(ms, list);
    print_structure(cout, ms, ms_name);
    cout << "Copy:" << endl;

    // test only
    list_type const ms2 = ms;
    typename list_type::const_iterator iter2 = ms2.end();
    typename list_type::const_iterator iter3(iter2);
    assert(iter2 == iter3);
    cout << "Size: " << ms.size() << endl;
    typename list_type::iterator iter4;
    //--iter4;

    list_type ms_copy(ms);
    print_structure(cout, ms_copy, ms_name);
    ms.clear();

    cout << "Build and destory:" << endl;
    build_and_destroy_skip_list(ms, ms_name, list);

//    BOOST_FOREACH(char c, list)
//        s2.insert(c);
//    cout << "Skip list structure:" << endl << internal_structure(s2) << endl;
//    BOOST_FOREACH(char c, list)
//    {
//        s2.erase(c);
//        cout << "Skip list structure:" << endl << internal_structure(s2) << endl;
//    }

////    cout << internal_structure(is1) << endl;
//    BOOST_FOREACH(char c, list)
//    {
//        is1.insert(c);
////        cout << internal_structure(is1) << endl;
//    }
//    cout << "Indexed skip list structure:" << endl << internal_structure(is1) << endl;

//    BOOST_FOREACH(char c, alphabet)
//        ms1.insert(c);
//    cout << "Multi-skip list structure:" << endl << internal_structure(ms1) << endl;
//    auto iter1 = ms1.find('Q');
//    assert(iter1 != ms1.end());
//    cout << "Found: " << *iter1 << endl;
//    auto iter2 = ms1.find('q');
//    assert(iter2 == ms1.end());
//    cout << "Not found: " << 'q' << endl;
//    cout << endl;

//    BOOST_FOREACH(char c, list)
//        ms2.insert(c);
//    cout << "Multi-skip list structure:" << endl << internal_structure(ms2) << endl;

//    BOOST_FOREACH(char c, list)
//        ims1.insert(c);
//    cout << "Indexed multi-skip list structure:" << endl << internal_structure(ims1) << endl;
}

