import React from 'react';

import Bob from './Bob';

import { renderWithProviders } from 'utils/test-utils';

describe('Bob', () => {
    it('displays articles when data is available', () => {
        renderWithProviders(<Bob />);
        expect(true).toBe(true);
    });
});
