//
// 'generic' graph structures and algorithms
//
  
#include <vector>
#include <array>
#include <queue>
#include <tuple>
#include <concepts>
#include <type_traits>

using vertex_id_t = std::size_t;
constexpr vertex_id_t invalid_vertex_id = vertex_id_t(-1);

inline bool valid_vertex_id(vertex_id_t v)
{
    return v != invalid_vertex_id;
}

template<typename GraphContainer> 
concept graph_container = requires (vertex_id_t v, GraphContainer G)
{
    G[v].begin();
    G[v].end();
    G.size();
    { *(G[v].begin())} -> std::convertible_to<vertex_id_t>;
};

template<typename D> struct result_recorder_distance
{
    using return_type = std::vector<D>;
    return_type rv;
    result_recorder_distance(size_t sz) : rv (sz, -1)
    {}
    void set_distance(vertex_id_t v, vertex_id_t u)
    {
        rv[v] = rv[u] + 1;
    }
    auto get_distance(vertex_id_t v) const
    {
        return rv[v];
    }
    void set_previous(vertex_id_t v, vertex_id_t u)
    {}
};

template<typename D, typename P = vertex_id_t> struct result_recorder_distance_previous
{
    using return_type = std::pair<std::vector<D>, std::vector<P>>;
    return_type rv;
//    result_recorder_distance_previous(size_t sz) : rv ( std::piecewise_construct, std::forward_as_tuple( sz, -1), std::forward_as_tuple(sz, -1))
    result_recorder_distance_previous(size_t sz) : rv ( std::piecewise_construct, std::forward_as_tuple( sz, -1), std::forward_as_tuple(sz, -1))
    {}    
    void set_distance(vertex_id_t v, vertex_id_t u)
    {
        rv.first[v] = rv.first[u] + 1;
    }
    auto get_distance(vertex_id_t v) const
    {
        return rv.first[v];
    }
    void set_previous(vertex_id_t v, P u)
    {
        rv.second[v] = u;
    }
    auto get_previous(vertex_id_t v) const
    {
        return rv.second[v];
    }
};

template<typename G, bool previous = false> 
requires graph_container<G> 
auto bfs(G const& g, vertex_id_t id_from) 
{
    // abstract the differences in return data required
    using recorder_t = typename std::conditional<previous, result_recorder_distance_previous<int>, result_recorder_distance<int>>::type;

    recorder_t              recorder(g.size());
    std::vector<bool>       visited(g.size());
    std::queue<vertex_id_t> q;
    q.push(id_from);
    recorder.set_distance(id_from, id_from); 
    visited[id_from] = true;
    while (!q.empty())
    {
        auto u = q.front(); q.pop();
        for (auto e : g[u])
        {
            if (!visited[e])
            {
                visited[e] = true;
                recorder.set_distance(e, u);
                recorder.set_previous(e, u);
                q.push(e);
            }
        }
    }
    return recorder.rv;
}  
#if 0
template<typename G, typename W, bool previous = false> 
requires graph_container<G> 
auto dijkstra(G const& g, vertex_id_t from)
{
    using weight_t  = W;
    // abstract the differences in return data required
    using recorder_t = typename std::conditional<previous, result_recorder_distance_previous<weight_t>, result_recorder_distance<weight_t>>::type;

    recorder_t recorder(g.size());
    using pq_t = std::pair<vertex_id_t, weight_t>;
    auto pq_t_cmp = [](auto& l, auto& r){ return l.second > r.second;};
    std::priority_queue<pq_t, std::vector<pq_t>, decltype(pq_t_cmp)> q(pq_t_cmp);
    q.push({from, 0});
    recorder.set_distance(from, from);
    while (!q.empty())
    {
        auto p = q.top(); q.pop();

        for( auto e : g[p.first])  
        {
            auto ev = g::vertex(e);
            if (recorder.get_distance(ev) == -1 || recorder.get_distance(ev) > recorder.get_distance(p.first) + g::weight(e))
            {
                recorder.set_distance(ev, recorder.get_distance(p.first) + g::weight(e));
                recorder.set_previous(ev, p.first);
                q.push({ev, recorder.get_distance(ev)});
            }
        }
    }

    return recorder.rv;
}
#endif
// helpers for adjacency lists
template<typename AdjacencyList, typename... ARGS> 
concept adjacency_list = requires (AdjacencyList g, ARGS... args)
{
    g.size();
    g.resize(size_t());
    g[0].emplace_back(0, args...);
};

template <typename G, typename... ARGS> 
requires adjacency_list<G, ARGS...>
void add_edge(G& g, vertex_id_t from, vertex_id_t to, ARGS... args )
{
    if (g.size() < from + 1)
        g.resize(from + 1);
    g[from].emplace_back(to, args...);
}

template <typename G, typename... ARGS> 
requires adjacency_list<G, ARGS...>
void add_edge_undirected(G& g, vertex_id_t from, vertex_id_t to, ARGS... args )
{
    add_edge(g, from, to, args...);
    add_edge(g, to, from, args...);
}
#if 0
// helper for grid of weights stored as a vector with a stride
//
template<typename W>
class weighted_grid
{
private:
    const std::vector<W> const& data_;
    const size_t stride_;
public:
    using edge_t = std::pair<vertex_id_t, W>;
    static vertex_id_t vertex(edge_t const& e) const
    {
        return e.first;
    }
    static W weight(edge_t const& e) const
    {
        return e.second;
    }   
    graph_t(std::vector<W> const& d, size_t s) : data_{ d }, stride_{s)}
    {}
    std::vector<edge_t> operator[](vertex_id_t v) const
    {
        std::vector<edge_t> rv;
        // left
        if (v % stride_ != 0)
            rv.emplace_back(v - 1, data_[v - 1]);
        // right
        if (v % stride_ != stride_ - 1)
            rv.emplace_back(v + 1, data_[v + 1]);
        // up
        if (v > stride_)
            rv.emplace_back(v - stride_, data_[v - stride_]);
        // down
        if (v < data_.size() - stride_)
            rv.emplace_back(v + stride_, data_[v + stride_]);
        return rv;
    }
    size_t size() const
    {
        return data_.size();
    }
};
#endif

// plain grid wrap for mazes etc.
// provide a fn to determine whether a path exists from adjacent otherwise valid nodes.
// fn of form bool V(T from, T to)
// where from and to are grid cell contents
//
template<typename T, typename V> class grid
{
private:
    const std::vector<T>& data_;
    const size_t stride_;
    const V vp_;
public:
    grid(std::vector<T> const& d, size_t s, V vp) : data_{ d }, stride_{s}, vp_{vp}
    {}
    std::vector<vertex_id_t> operator[](vertex_id_t v) const
    {
        std::vector<vertex_id_t> rv;
        // left
        if (v % stride_ != 0 && vp_(data_[v], data_[v - 1]))
            rv.emplace_back(v - 1);
        // right
        if (v % stride_ != stride_ - 1 && vp_(data_[v], data_[v + 1]))
            rv.emplace_back(v + 1);
        // up
        if (v > stride_ && vp_(data_[v], data_[v - stride_]))
            rv.emplace_back(v - stride_);
        // down
        if (v < data_.size() - stride_ && vp_(data_[v], data_[v + stride_]))
            rv.emplace_back(v + stride_);
        return rv;
    }
    std::vector<vertex_id_t> two_step(vertex_id_t v) const
    {
        std::vector<vertex_id_t> rv;
        auto [x, y] = to_xy(v);
        // left
        if (x > 1 && !vp_(data_[v], data_[v - 1]) && vp_(data_[v], data_[v - 2]))
            rv.emplace_back(v - 2);
        // right
        if (x < stride_ - 2 && !vp_(data_[v], data_[v + 1]) && vp_(data_[v], data_[v + 2]))
            rv.emplace_back(v + 2);
        // up
        if (y > 1 && !vp_(data_[v], data_[v - stride_]) && vp_(data_[v], data_[v - 2 * stride_]))
            rv.emplace_back(v - 2 *stride_);
        // down
        if (y < stride_ - 2 && !vp_(data_[v], data_[v + stride_]) && vp_(data_[v], data_[v + 2 * stride_]))
            rv.emplace_back(v + 2 * stride_);
        return rv;
    }
    template<int N> auto make_viable(vertex_id_t v) const
    {
        std::vector<std::pair<vertex_id_t, int>> viable;
        auto[x, y] = to_xy(v);
        for(int dx = -N; dx <= N; ++dx)
        {
            for(int dy = -N; dy <= N; ++dy)
            {
                int d{std::abs(dx) + std::abs(dy)};
                if(d <= N && d > 1 && in_bounds(x+dx, y+dy))
                    viable.emplace_back(from_xy(x + dx, y + dy), d );
            }
        }
        return viable;
    }
    template<int N> auto n_step(vertex_id_t v) const
    {
        std::vector<std::pair<vertex_id_t, int>> rv;
        auto viable{make_viable<N>(v)};
        for(auto [vt, d]: viable)
            if(vp_(data_[v], data_[vt]))
                rv.emplace_back(vt, d);
        return rv;
    }
    size_t size() const
    {
        return data_.size();
    }
    std::pair<size_t, size_t> to_xy(vertex_id_t v) const
    {
        return { v % stride_, v / stride_ };
    }
    vertex_id_t from_xy(size_t x, size_t y) const
    {
        return x +  y * stride_;
    }
    bool in_bounds(int x, int y) const
    {
        return x >= 0 && x < stride_ &&
            y >= 0 && y < stride_;
    }
};

// plain grid wrap for mazes etc.
// provide a fn to determine whether a path exists from adjacent otherwise valid nodes.
// fn of form bool V(T from, T to)
// where from and to are vertex ids.
//
template<typename T, typename V> class grid_direct
{
private:
    const std::vector<T>& data_;
    const size_t stride_;
    const V vp_;
public:
    grid_direct(std::vector<T> const& d, size_t s, V vp) : data_{ d }, stride_{ s }, vp_{ vp }
    {}
    std::vector<vertex_id_t> operator[](vertex_id_t v) const
    {
        std::vector<vertex_id_t> rv;
        // left
        if (v % stride_ != 0 && vp_(v, v - 1))
            rv.emplace_back(v - 1);
        // right
        if (v % stride_ != stride_ - 1 && vp_( v, v + 1))
            rv.emplace_back(v + 1);
        // up
        if (v > stride_ && vp_(v, v - stride_))
            rv.emplace_back(v - stride_);
        // down
        if (v < data_.size() - stride_ && vp_(v, v + stride_))
            rv.emplace_back(v + stride_);
     return rv;
    }
    size_t size() const
    {
        return data_.size();
    }
    size_t stride () const
    {
        return stride_;
    }
    T value(vertex_id_t v) const
    {
        return data_[v];
    }
};

// plain grid wrap for mazes etc.
// assumes a border
// provide a fn to determine whether a node is valid.
// fn of form bool V(T to)
//
template<typename T, typename V> class grid_direct_border
{
private:
    const std::vector<T>& data_;
    const size_t stride_;
    const V vp_;
public:
    grid_direct_border(std::vector<T> const& d, size_t s, V vp) : data_{ d }, stride_{ s }, vp_{ vp }
    {}
    std::vector<vertex_id_t> operator[](vertex_id_t v) const
    {
        std::vector<vertex_id_t> rv;
        // left
        if ( vp_(v - 1))
            rv.emplace_back(v - 1);
        // right
        if (vp_(v + 1))
            rv.emplace_back(v + 1);
        // up
        if (vp_(v - stride_))
            rv.emplace_back(v - stride_);
        // down
        if (vp_(v + stride_))
            rv.emplace_back(v + stride_);
     return rv;
    }
    size_t size() const
    {
        return data_.size();
    }
    size_t stride () const
    {
        return stride_;
    }
    T value(vertex_id_t v) const
    {
        return data_[v];
    }
};

// plain grid wrap for mazes etc.
// assumes a border
// provide a fn to determine whether a node is valid.
// fn of form bool V(T to)
// checks up/left/right/down, IOW 'reading order'
//
template<typename T, typename V> class grid_direct_border_ro
{
private:
    const std::vector<T>& data_;
    const size_t stride_;
    const V vp_;
public:
    grid_direct_border_ro(std::vector<T> const& d, size_t s, V vp) : data_{ d }, stride_{ s }, vp_{ vp }
    {}
    std::vector<vertex_id_t> operator[](vertex_id_t v) const
    {
        std::vector<vertex_id_t> rv;
        // up
        if (vp_(v - stride_))
            rv.emplace_back(v - stride_);
        // left
        if ( vp_(v - 1))
            rv.emplace_back(v - 1);
        // right
        if (vp_(v + 1))
            rv.emplace_back(v + 1);
        // down
        if (vp_(v + stride_))
            rv.emplace_back(v + stride_);
        return rv;
    }
    size_t size() const
    {
        return data_.size();
    }
    size_t stride () const
    {
        return stride_;
    }
    T value(vertex_id_t v) const
    {
        return data_[v];
    }
};

// plain grid wrap for mazes etc.
// provide a fn to determine whether a path exists from adjacent otherwise valid nodes.
// fn of form bool V(T from, T to)
// where from and to are grid cell contents
// always returns 4 vertices, -1 for no connection;
//
template<typename T, typename V> class grid_4
{
private:
    const std::vector<T>& data_;
    const size_t stride_;
    const V vp_;
public:
    grid_4(std::vector<T> const& d, size_t s, V vp) : data_{ d }, stride_{s}, vp_{vp}
    {}

    std::array<vertex_id_t, 4> operator[](vertex_id_t v) const
    {
        std::array<vertex_id_t, 4> rv;
        rv.fill(invalid_vertex_id);
        // left
        if (v % stride_ != 0 && vp_(data_[v], data_[v - 1]))
            rv[0] = v - 1;
        // up
        if (v > stride_ && vp_(data_[v], data_[v - stride_]))
            rv[1] = v - stride_;
        // right
        if (v % stride_ != stride_ - 1 && vp_(data_[v], data_[v + 1]))
            rv[2] = v + 1;
        // down
        if (v < data_.size() - stride_ && vp_(data_[v], data_[v + stride_]))
            rv[3] = v + stride_;
        return rv;
    }
    size_t size() const
    {
        return data_.size();
    }
    size_t stride() const
    {
        return stride_;
    }
    std::vector<T> const& data() const
    {
        return data_;
    }
};

// grid with no preconceived data...
//
template<typename V> class grid_virtual
{
private:
    const size_t stride_;
    const V vp_;
public:
    grid_virtual(size_t s, V vp) : stride_{ s }, vp_{ vp }
    {}
    std::vector<vertex_id_t> operator[](vertex_id_t v) const
    {
        std::vector<vertex_id_t> rv;
        // left
        if (v % stride_ != 0 && vp_(v, v - 1))
            rv.emplace_back(v - 1);
        // right
        if (v % stride_ != stride_ - 1 && vp_( v, v + 1))
            rv.emplace_back(v + 1);
        // up
        if (v > stride_ && vp_(v, v - stride_))
            rv.emplace_back(v - stride_);
        // down
        if ( vp_(v, v + stride_))
            rv.emplace_back(v + stride_);
        return rv;
    }
    size_t stride () const
    {
        return stride_;
    }
    size_t size() const
    {
        return stride_ * stride_;
    }
};

// grid that looks all around
//
class grid_direct8
{
private:
    const size_t length_;
    const size_t stride_;

public:
    grid_direct8(size_t l, size_t s) : length_{l}, stride_{ s }
    {}
    std::vector<vertex_id_t> operator[](vertex_id_t v) const
    {
        std::vector<vertex_id_t> rv;
        unsigned k = 0;
        bool l = v % stride_ != 0;
        bool r = v % stride_ != stride_ - 1;
        bool u = v > stride_;
        bool d = v < length_ - stride_;
        if(l)
        {
            if(u)
                rv.emplace_back(v - stride_ - 1);
            rv.emplace_back(v - 1);
            if(d)
                rv.emplace_back(v + stride_ - 1);
        }
        if(u)
            rv.emplace_back(v - stride_);
        if(d)
            rv.emplace_back(v + stride_);
        if(r)
        {
            if(u)
                rv.emplace_back(v - stride_ + 1);
            rv.emplace_back(v + 1);
            if(d)
                rv.emplace_back(v + stride_ + 1);
        }
        return rv;
    }
    size_t stride () const
    {
        return stride_;
    }
    size_t size() const
    {
        return length_;
    }
};
