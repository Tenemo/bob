import express from 'express';

export const router = express.Router();

/** GET /health-check - Check service health */
router.get('/health-check', (_req, res) => res.send('OK'));
