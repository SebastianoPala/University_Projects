package com.unipi.PlayerHive.DTO.containers;

import com.unipi.PlayerHive.DTO.reviews.GameReviewDTO;
import lombok.AllArgsConstructor;
import lombok.Getter;

import java.util.List;

@Getter
@AllArgsConstructor
public class GameReviewContainerDTO {

    private List<GameReviewDTO> reviews;

    private int numPages;

    private boolean isLastPage;
}
