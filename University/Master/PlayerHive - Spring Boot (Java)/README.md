# PlayerHive

PlayerHive is a social gaming platform developed as the final project for the "Large-Scale and Multi-Structured Databases" exam at the University of Pisa (Master's Degree in Computer Engineering, Academic Year 2025/2026).

The platform is built around the principle of "for gamers, by gamers", providing users with tools to discover new titles, track their personal game collections, publish reviews, and connect with a community of players. The system is engineered to handle large-scale datasets and relies on a polyglot persistence architecture to optimize different data access patterns.

## Tech Stack

* **Backend Framework:** Spring Boot 3.5
* **Authentication:** Stateless JWT (JSON Web Token)
* **Document Database:** MongoDB
* **Graph Database:** Neo4j

## Database Architecture

The project implements a distributed database design following an AP (Availability and Partition Tolerance) model, prioritizing system responsiveness and uptime over strict real-time consistency. Cross-database transactions are managed at the application level to ensure eventual consistency.

* **MongoDB:** Utilized as the primary document store for the `users`, `games` (over 120,000 entries), and `reviews` collections. The architecture accounts for a three-node replica set configuration with asymmetric election priorities. The sharding strategy employs hashed sharding for users and games to ensure uniform distribution, and targeted sharding by `game_id` for reviews to optimize read latency on highly accessed game profiles.
* **Neo4j:** Utilized to model the platform's highly interconnected data, specifically the bidirectional social graph (`:FRIENDS_WITH`) and the user-to-game library relationships (`:PLAYED`). This allows for efficient execution of complex, multi-hop recommendation algorithms that would be highly inefficient in a standard document store.

## Main Features

* **Game Catalogue:** Browse and search through a comprehensive database of games, viewing metadata, community metrics, and paginated user reviews.
* **Personal Library:** Track gaming progress by logging playtime hours and unlocked achievements for owned titles.
* **Review System:** Publish critical feedback and numerical scores for games, with integrated management for updating or deleting past reviews.
* **Social Networking:** Send, accept, or deny friend requests to build a personal network, allowing users to view each other's profiles, libraries, and activities.
* **Advanced Analytics & Discovery:**
* **Social Recommendations:** Collaborative filtering to suggest games popular within a user's direct friend group.
* **Gaming Twins:** Identification of highly compatible users based on shared library overlaps using Jaccard Similarity.
* **Hidden Gems:** Algorithmic discovery of niche titles that show high engagement locally but low popularity globally.
* **Community Leaderboards:** Aggregation pipelines to rank users based on specific engagement metrics (e.g., Hardcore Gamers, Keyboard Warriors).



## Administrative Suite

The platform includes elevated privileges for administrative accounts to curate the dataset and monitor the ecosystem:

* Add, edit, or delete entries in the global game catalog.
* Perform cascading deletions of user accounts to moderate the community.
* Access analytical dashboards extracting global platform trends, such as genre performance, OS compatibility metrics, and historical release year statistics.
