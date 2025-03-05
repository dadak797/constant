import express from "express";
import morgan from "morgan";
import path from "path";

import rootRouter from "./routers/rootRouter.js";
import { createServer } from "http";


const __dirname = path.resolve();

const app = express();
const httpServer = createServer(app);
const logger = morgan("dev");

// Setup COOP and COEP headers
app.use((req, res, next) => {
  res.setHeader('Cross-Origin-Opener-Policy', 'same-origin');
  res.setHeader('Cross-Origin-Embedder-Policy', 'require-corp');
  next();
});

// Static folder
app.use('/wasm', express.static(path.join(__dirname, 'wasm')));
app.use(`/${process.env.UPLOAD_PATH}`, express.static(path.join(__dirname, `${process.env.UPLOAD_PATH}`)));

app.use(logger);
app.use(express.urlencoded({ extended: true }));  // For encoding of req.body
app.use(express.json());

app.use("/", rootRouter);

export default httpServer;