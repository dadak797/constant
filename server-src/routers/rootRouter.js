import express from "express";
import { getHome } from "../controllers/rootController.js";


const rootRouter = express.Router();

rootRouter.get("/", getHome);

export default rootRouter;