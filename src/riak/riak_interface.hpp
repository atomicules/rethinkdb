#ifndef __RIAK_INTERFACE_HPP__
#define __RIAK_INTERFACE_HPP__

#include "riak/structures.hpp"
#include "store_manager.hpp"
#include <boost/ptr_container/ptr_map.hpp>
#include "http/json.hpp"

#define RIAK_LIST_KEYS_BATCH_FACTOR 10

/* The riak_interface_t is class with all the functions needed to implement a
 * riak server, in most cases the calls are obviously isomorphic to requests
 * that can be sent to a server. A notable exception to this is link walking
 * which will make multiple calls to the interface. */

namespace riak {

namespace json = json_spirit;

class bucket_iterator_t {
private:
    store_manager_t<std::list<std::string> >::const_iterator it;

public:
    bucket_iterator_t(store_manager_t<std::list<std::string> >::const_iterator const &it) 
        : it(it)
    { }

public:
    bool operator==(bucket_iterator_t const &other) { return it == other.it; }
    bool operator!=(bucket_iterator_t const &other) { return !operator==(other); }
    bucket_iterator_t operator++() { it++; return *this; }
    bucket_iterator_t operator++(int) { it++; return *this; }
    bucket_t operator*() { return boost::get<riak_store_metadata_t>(it->second->store_metadata); }
    bucket_t *operator->() { return &boost::get<riak_store_metadata_t>(it->second->store_metadata); };
};

class riak_interface_t {
private:
    store_manager_t<std::list<std::string> > *store_manager;

    typedef boost::ptr_map<std::list<std::string>, btree_slice_t> slice_map_t;

    slice_map_t slice_map;

    btree_slice_t *get_slice(std::list<std::string>);
    btree_slice_t *create_slice(std::list<std::string>);
public:
    riak_interface_t(store_manager_t<std::list<std::string> > *store_manager)
        : store_manager(store_manager)
    { }

private:

public:
    // Bucket operations:

    // Get a bucket by name
    boost::optional<bucket_t> get_bucket(std::string);
    // Set the properties of a bucket
    void set_bucket(std::string, bucket_t);
    // Get all the buckets
    std::pair<bucket_iterator_t, bucket_iterator_t> buckets();



    // Object operations:
    
    // Get all the keys in a bucket
    object_iterator_t objects(std::string);
    // Get a single object
    const object_t get_object(std::string, std::string, std::pair<int,int> range = std::make_pair(-1, -1));
    // Store an object
    void store_object(std::string, object_t);
    // Delete an object
    bool delete_object(std::string, std::string);

    //run a mapreduce job
    json::mObject mapreduce(json::mValue &);

    //make a key (which is guarunteed to be unique)
    std::string gen_key();


};

} //namespace riak

#endif
