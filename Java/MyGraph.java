import java.util.*;

/**
 * A representation of a graph.
 * Assumes that we do not have negative cost edges in the graph.
 */
public class MyGraph implements Graph {

    private Map<Vertex, ArrayList<Edge>> map;

    /**
     * Creates a MyGraph object with the given collection of vertices
     * and the given collection of edges.
     * @param v a collection of the vertices in this graph
     * @param e a collection of the edges in this graph
     */
    public MyGraph(Collection<Vertex> v, Collection<Edge> e) {

        this.map = new HashMap<Vertex, ArrayList<Edge>>();
        addVertices(v);
        try {
            addEdges(e);
        } catch (NegativeWeightException | NoVertexException e1) {
            e1.printStackTrace();
        }
    }

    /**
     * Adds edges e into the map, does not add duplicates
     * @param e a collection of edges
     * @throws NegativeWeightException on an edge with a negative weight
     * @throws NoVertexException on an edge with a missing vertex
     */
    private void addEdges(Collection<Edge> e) throws NegativeWeightException, NoVertexException {
        for (Edge cur : e) {
            if (!map.containsKey(cur.getSource()) || !map.containsKey(cur.getDestination())) {
                throw new NoVertexException("addEdges: Cannot add edge to graph without already having to and from vertex.");
            }
            if (cur.getWeight() < 0) {
                throw new NegativeWeightException("addEdges: Cannot add edge to graph with a negative weight.");
            }

            ArrayList<Edge> edgeList = map.get(cur.getSource());
            if (edgeList == null) {
                edgeList = new ArrayList<>();
                edgeList.add(cur);
                map.replace(cur.getSource(), edgeList);
            } else if (!edgeList.contains(cur)){
                edgeList.add(cur);
                map.replace(cur.getSource(), edgeList);
            }
        }
    }

    /**
     * Adds vertices v to map, does not add duplicates
     * @param v a collection of vertices
     */
    private void addVertices(Collection<Vertex> v) {
        for (Vertex cur : v) {
            if (!map.containsKey(cur)) {
                map.put(cur, null);
            }
        }
    }

    /** 
     * Return the collection of vertices of this graph
     * @return the vertices as a collection (which is anything iterable)
     */
    public Collection<Vertex> vertices() {
        Collection<Vertex> col = new HashSet<Vertex>();
        col.addAll(map.keySet());
        return col;
    }

    /** 
     * Return the collection of edges of this graph
     * @return the edges as a collection (which is anything iterable)
     */
    public Collection<Edge> edges() {
        Collection<Edge> col = new HashSet<Edge>();
        for (List<Edge> cur : map.values()) {
            if (cur != null) {
                col.addAll(cur);
            }
        }
        return col;
    }

    /**
     * Return a collection of vertices adjacent to a given vertex v.
     *   i.e., the set of all vertices w where edges v -> w exist in the graph.
     * Return an empty collection if there are no adjacent vertices.
     * @param v one of the vertices in the graph
     * @return an iterable collection of vertices adjacent to v in the graph
     * @throws IllegalArgumentException if v does not exist.
     */
    public Collection<Vertex> adjacentVertices(Vertex v) {
        if (v == null) {
            throw new IllegalArgumentException("adjacentVertices: v cannot be null");
        }
        if (!map.containsKey(v)) {
            throw new IllegalArgumentException("adjacentVertices: v must be in the graph");
        }

        Collection<Vertex> col = new HashSet<Vertex>();
        ArrayList<Edge> edges = map.get(v);
        if (edges != null) {
            for (Edge cur : edges) {
                if (cur != null)
                    col.add(cur.getDestination());
            }
        }

        return col;
    }

    /**
     * Test whether vertex b is adjacent to vertex a (i.e. a -> b) in a directed graph.
     * Assumes that we do not have negative cost edges in the graph.
     * @param a one vertex
     * @param b another vertex
     * @return cost of edge if there is a directed edge from a to b in the graph, 
     * return -1 otherwise.
     * @throws IllegalArgumentException if a or b do not exist.
     */
    public int edgeCost(Vertex a, Vertex b) {
        if (a == null || b == null) {
            throw new IllegalArgumentException("edgeCost: Cannot accept null vertices.");
        }
        if (!map.containsKey(a) || !map.containsKey(b)) {
            throw new IllegalArgumentException("edgeCost: Both vertices must be in graph.");
        }

        int cost = -1;
        ArrayList<Edge> edges = map.get(a);
        for (Edge cur : edges) {
            if (cur.getDestination().equals(b)) {
                cost = cur.getWeight();
            }
        }

        return cost;
    }

    /**
     * Returns the shortest path between a and b using Dijkstra's Algorithm
     * Assumes a and b are in the graph
     * @param a starting vertex
     * @param b ending vertex
     */
    public Path shortestPath(Vertex a, Vertex b) {
        List<Vertex> pathList = new ArrayList<Vertex>();

        if (a.equals(b)) {
            pathList.add(a);
            return new Path(pathList, 0);
        }

        pathList.add(a);
        Map<Vertex, Path> map = new HashMap<>();
        map.put(a, new Path(pathList, 0));
        PriorityQueue<PriorityVertex> q = new PriorityQueue<>();
        for (Vertex v : vertices()) {
            if (!v.equals(a)) {
                map.put(v, new Path(new ArrayList<>(), Integer.MAX_VALUE));
            }
            q.add(new PriorityVertex(v, map.get(v).cost));
        }

        while (!q.isEmpty()) {
            PriorityVertex cur = q.poll();
            for (Vertex next : adjacentVertices(cur.vertex)) {
                int newCost = map.get(cur.vertex).cost + edgeCost(cur.vertex, next);
                int oldCost = map.get(next).cost;
                if (newCost < oldCost) {
                    Path newP = new Path( new ArrayList<>(), newCost);
                    newP.vertices.addAll(map.get(cur.vertex).vertices);
                    newP.vertices.add(next);
                    map.replace(next, newP);

                    q.remove(next);
                    q.add(new PriorityVertex(next, newCost));
                }
            }
            if (cur.equals(b)) {
                return map.get(b);
            }
        }

        return null;
    }

}

/**
 * priorityVertex allows use of data structures that require compareTo for vertices
 */
class PriorityVertex implements Comparable {
    public Vertex vertex;
    public int cost;

    PriorityVertex(Vertex v, int c) {
        vertex = v;
        cost = c;
    }

    @Override
    public int compareTo(Object o) {
        if (cost > ((PriorityVertex) o).cost) {
            return 1;
        } else if (cost == ((PriorityVertex) o).cost) {
            return 0;
        } else {
            return -1;
        }
    }

    public boolean equals(Object obj) {
        return vertex.equals(obj);
    }
}


/**
 * Created by sleisle on 5/8/2017.
 * NegativeWeightException is an exception thrown by MyGraph on a negatively weighted edge.
 */
class NegativeWeightException extends Exception {
    public NegativeWeightException() {};

    public NegativeWeightException(String message) {
        super(message);
    }

    public NegativeWeightException(Throwable cause) {
        super(cause);
    }

    public NegativeWeightException(String message, Throwable cause) {
        super(message, cause);
    }
}

/**
 * Created by sleisle on 5/8/2017.
 * NoVertexException is an exception thrown by MyGraph on an unconnected edge.
 */
class NoVertexException extends Exception {
    public NoVertexException() {};

    public NoVertexException(String message) {
        super(message);
    }

    public NoVertexException(Throwable cause) {
        super(cause);
    }

    public NoVertexException(String message, Throwable cause) {
        super(message, cause);
    }
}
