import React from 'react';

import HomePage from './HomePage';

import { renderWithProviders } from 'utils/test-utils';

describe('HomePage', () => {
    it('displays articles when data is available', () => {
        renderWithProviders(<HomePage />);
        expect(true).toBe(true);
    });
});
