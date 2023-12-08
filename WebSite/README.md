# How to Run

## Prerequisites

1. **Node.js and npm:** Make sure you have Node.js and npm installed. You can download them from [Node.js official website](https://nodejs.org/).

2. **Angular CLI:** Install Angular CLI globally using the following command:

    ```bash
    npm install -g @angular/cli
    ```

3. **PostgreSQL:** Install PostgreSQL and have it running on your machine. You can download it from [PostgreSQL official website](https://www.postgresql.org/download/).

4. **pgAdmin:** Install pgAdmin, a PostgreSQL administration tool, from [pgAdmin official website](https://www.pgadmin.org/download/).

## Backend Setup

1. **Clone the backend repository:**

    ```bash
    git clone ...
    cd CacheCompass_Tracker\WebSite\geocache-app
    ```

2. **Install dependencies:**

    ```bash
    npm install
    ```

3. **Configure Database:**

    - Create your database at pgAdmin and open `common/envs/development.env` and update the database connection details.

    ```json
    PORT=3000
    BASE_URL=http://localhost:3000

    DATABASE_HOST= ...
    DATABASE_NAME= ...
    DATABASE_USER= ...
    DATABASE_PASSWORD= ...
    DATABASE_PORT= ...
    ```

5. **Start the Backend Server:**

    ```bash
    npm run start
    ```

## Frontend Setup

1. **Clone the frontend repository:**

    ```bash
    git clone ...
    cd CacheCompass_Tracker\WebSite\front\geocache
    ```

2. **Install dependencies:**

    ```bash
    npm install
    ```

4. **Run the Angular App:**

    ```bash
    ng serve -o
    ```

5. **Open in Browser:**

    Open your browser and navigate to `http://localhost:4200/` to view the Cache Compress Tracker.


