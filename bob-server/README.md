# expressplate

Express.js REST API opinionated boilerplate designed for building scalable and maintainable web applications. This project includes a pre-configured environment with TypeScript, Express.js, and various tools to ensure code quality and consistency.

## Features

-   **TypeScript**
-   **Express.js**
-   **Babel**: Babel can convert TypeScript code into a backward compatible version of JavaScript in current and older environments.
-   **ESLint & Prettier**
-   **Husky**
-   **Jest**
-   **Winston & Morgan**: Robust logging

## Installation

To get started with `expressplate`, clone the repository and install dependencies:

```bash
git clone https://github.com/Tenemo/expressplate.git
cd expressplate
npm install
```

## Configuration

Adjust the `.env` file in the root directory to match your environment variables. A `.env.sample` file is provided as an example.

## Development

To start the development server with hot reload:

```bash
npm run dev:ts
```

## Build

To compile TypeScript to JavaScript and build the project for production:

```bash
npm run build
```

## Running Tests

To run tests using Jest:

```bash
npm test
```

## Code Quality

Lint your code and fix formatting issues:

```bash
npm run eslint:fix
```

## Deployment

Deploy the built application by starting the server with:

```bash
npm start
```

This will run the compiled JavaScript code from the `dist` directory.

## Logging

Logs are configured to rotate daily with separate files for combined logs and error logs. Check the `logs` directory.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request for any improvements.

## License

This project is under MIT license. The MIT License is a permissive free software license originating at the Massachusetts Institute of Technology (MIT). It is simple and easy to understand and it places almost no restrictions on what you can do with this project.
