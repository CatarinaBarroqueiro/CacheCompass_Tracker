# Building a Web App with Angular Frontend and Node.js TypeORM PostgreSQL Backend

## Overview
This report provides an overview of the process involved in creating a web application with Angular frontend and Node.js backend. The tutorial followed for this project can be found [here](https://betterprogramming.pub/nest-js-project-with-typeorm-and-postgres-ce6b5afac3be).

## Prerequisites
- Basic understanding of Node.js, TypeScript, and PostgreSQL.
- Visual Studio Code as the code editor.

## How to Run

### Backend Setup
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
    - Create your database at pgAdmin and open `common/envs/development.env` to update the database connection details.
    ```json
    PORT=3000
    BASE_URL=http://localhost:3000

    DATABASE_HOST= ...
    DATABASE_NAME= ...
    DATABASE_USER= ...
    DATABASE_PASSWORD= ...
    DATABASE_PORT= ...
    ```

4. **Start the Backend Server:**
    ```bash
    npm run start
    ```

### Frontend Setup
1. **Clone the frontend repository:**
    ```bash
    git clone ...
    cd CacheCompass_Tracker\WebSite\front\geocache
    ```

2. **Install dependencies:**
    ```bash
    npm install
    ```

3. **Run the Angular App:**
    ```bash
    ng serve -o
    ```

4. **Open in Browser:**
    Open your browser and navigate to `http://localhost:4200/` to view the Cache Compress Tracker.

## Backend Challenges
- **CORS**: Due to backend blocking frontend access, CORS was enabled. This step addresses difficulties faced during development.

## Additional Deployment on AWS
### Tools & Stacks
- NestJS
- AWS EC2
- AWS RDS
- PM2

### Deployment Steps
1. **Create an Ubuntu server EC2 instance on AWS:**
    - Launch an instance with Ubuntu 20.04 LTS.
    - Configure security rules for SSH and API access.

2. **Connect to the EC2 instance:**
    - Use SSH to connect to the instance.

3. **Install dependencies on the EC2 instance:**
    ```bash
    sudo apt-get install nodejs git npm
    ```

4. **Install yarn & pm2:**
    ```bash
    sudo npm install yarn pm2 -g
    ```

5. **Clone the NestJS project:**
    ```bash
    git clone https://github.com/verdotte/nest-crud-api.git
    cd nest-crud-api
    yarn install
    ```

6. **Create a Postgres database instance on AWS RDS:**
    - Set up a Postgres database with proper security rules.

7. **Configure the app.json file:**
    - Create an `app.json` file with PM2 configurations.

8. **Create a .env file:**
    - Add database credentials to the `.env` file.

9. **Run the app using PM2:**
    ```bash
    sudo yarn pm2:deploy:app
    ```

10. **Troubleshooting:**
    - **SSL Error:** If encountering a "no pg_hba.conf entry" error, set `rds.force_ssl` to 0 in the RDS configuration.
    - **UA.PT Blocking:** If UA.PT blocks access to the AWS EC2 IP, consider using a mobile network for access.

11. **Test the API:**
    - Execute commands to build and start the app.
    ```bash
    sudo yarn build && yarn start
    ```

12. **Access API documentation:**
    - Visit the API documentation endpoint: [API Documentation](http://ec2-54-87-3-233.compute-1.amazonaws.com:3000/v1/doc-api)

13. **Conclusion:**
    - The deployment on AWS EC2 involves multiple steps, including instance creation, security rule configuration, and SSH connection. This tutorial covers essential deployment steps, while additional topics like CI/CD, HTTPS setup, and domain connection are not covered but provide a solid starting point.
